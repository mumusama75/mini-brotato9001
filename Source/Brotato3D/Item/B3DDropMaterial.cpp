#include "Item/B3DDropMaterial.h"
#include "Core/B3DAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AB3DDropMaterial::AB3DDropMaterial()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->InitSphereRadius(24.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionSphere->SetGenerateOverlapEvents(true);
    CollisionSphere->SetCanEverAffectNavigation(false);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetRelativeScale3D(FVector(0.18f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereAsset.Succeeded())
    {
        MeshComp->SetStaticMesh(SphereAsset.Object);
    }
}

void AB3DDropMaterial::BeginPlay()
{
    Super::BeginPlay();

    UMaterialInterface* BaseMat = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")));
    if (BaseMat && MeshComp)
    {
        UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMat, this);
        Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 1.0f, 0.40f, 1.0f));
        MeshComp->SetMaterial(0, Mat);
    }

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AB3DDropMaterial::OnOverlap);
}

void AB3DDropMaterial::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Continuous idle rotation
    AddActorLocalRotation(FRotator(0.0f, 120.0f * DeltaTime, 0.0f));

    // Magnet check
    if (!bAttracting)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
        if (PlayerPawn)
        {
            const float DistSq = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());
            // Default 300cm attraction radius
            if (DistSq < FMath::Square(320.0f))
            {
                bAttracting = true;
                TargetPlayer = PlayerPawn;
            }
        }
    }

    if (bAttracting && TargetPlayer.IsValid())
    {
        FVector Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
        const float Dist = Dir.Size();
        if (Dist < 40.0f)
        {
            // Collect!
            if (UB3DAttributeComponent* Attrib = TargetPlayer->FindComponentByClass<UB3DAttributeComponent>())
            {
                Attrib->AddMaterials(MaterialValue);
            }
            Destroy();
            return;
        }

        Dir.Normalize();
        AddActorWorldOffset(Dir * MagnetSpeed * DeltaTime);
    }
}

void AB3DDropMaterial::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (UB3DAttributeComponent* Attrib = OtherActor->FindComponentByClass<UB3DAttributeComponent>())
        {
            Attrib->AddMaterials(MaterialValue);
        }
        Destroy();
    }
}
