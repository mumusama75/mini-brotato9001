#include "Weapon/B3DProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AB3DProjectile::AB3DProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    RootComponent = SphereCollision;
    SphereCollision->InitSphereRadius(16.0f);
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SphereCollision->SetGenerateOverlapEvents(true);
    SphereCollision->SetCanEverAffectNavigation(false);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetRelativeScale3D(FVector(0.24f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereAsset.Succeeded())
    {
        MeshComp->SetStaticMesh(SphereAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMatFinder.Succeeded())
    {
        MeshComp->SetMaterial(0, BaseMatFinder.Object);
    }

    InitialLifeSpan = 5.0f;
}

void AB3DProjectile::BeginPlay()
{
    Super::BeginPlay();
    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AB3DProjectile::OnOverlapBegin);
}

void AB3DProjectile::Initialize(FVector Direction, float InSpeed, float InDamage, bool bCrit, int32 InPierce, int32 InBounce, float InHoming, AActor* Target)
{
    Speed = InSpeed;
    Damage = InDamage;
    bIsCritical = bCrit;
    PierceLeft = InPierce;
    BouncesLeft = InBounce;
    HomingStrength = InHoming;
    HomingTarget = Target;

    Direction.Z = 0.0f;
    Direction.Normalize();
    MoveVelocity = Direction * Speed;

    SetActorRotation(Direction.Rotation());

    // Scale mesh if critical
    if (bIsCritical)
    {
        MeshComp->SetRelativeScale3D(FVector(0.36f));
    }

    if (UMaterialInstanceDynamic* ProjMat = MeshComp->CreateDynamicMaterialInstance(0))
    {
        ProjMat->SetVectorParameterValue(TEXT("Color"), bIsCritical ? FLinearColor(1.0f, 0.4f, 0.1f, 1.0f) : FLinearColor(1.0f, 0.92f, 0.20f, 1.0f));
    }
}

void AB3DProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Homing steering
    if (HomingStrength > 0.0f && IsValid(HomingTarget))
    {
        FVector ToTarget = HomingTarget->GetActorLocation() - GetActorLocation();
        ToTarget.Z = 0.0f;
        ToTarget.Normalize();

        FVector CurrentDir = MoveVelocity.GetSafeNormal();
        FVector NewDir = FMath::VInterpTo(CurrentDir, ToTarget, DeltaTime, HomingStrength * 8.0f);
        NewDir.Normalize();
        MoveVelocity = NewDir * Speed;
        SetActorRotation(NewDir.Rotation());
    }

    FHitResult HitResult;
    AddActorWorldOffset(MoveVelocity * DeltaTime, true, &HitResult);

    if (HitResult.bBlockingHit)
    {
        if (BouncesLeft > 0)
        {
            --BouncesLeft;
            FVector Normal = HitResult.ImpactNormal;
            Normal.Z = 0.0f;
            Normal.Normalize();
            MoveVelocity = FMath::GetReflectionVector(MoveVelocity, Normal);
            SetActorRotation(MoveVelocity.Rotation());
        }
        else if (HitResult.GetActor() && !HitResult.GetActor()->ActorHasTag(TEXT("Player")))
        {
            Destroy();
        }
    }
}

void AB3DProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
    {
        return;
    }

    if (HitActors.Contains(OtherActor))
    {
        return;
    }

    // Only hit enemies
    if (OtherActor->ActorHasTag(TEXT("Enemy")))
    {
        HitActors.Add(OtherActor);

        FDamageEvent DamageEvent;
        OtherActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);

        // Apply kinematic knockback
        FVector KnockDir = MoveVelocity.GetSafeNormal();
        KnockDir.Z = 0.0f;
        FHitResult KnockHit;
        OtherActor->AddActorWorldOffset(KnockDir * 25.0f, true, &KnockHit);

        if (PierceLeft > 0)
        {
            --PierceLeft;
        }
        else
        {
            Destroy();
        }
    }
}
