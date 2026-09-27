#include "Enemy/B3DEnemyBase.h"
#include "Core/B3DAttributeComponent.h"
#include "Item/B3DDropMaterial.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AB3DEnemyBase::AB3DEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(35.0f, 45.0f);
    CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));
    CapsuleComp->SetCanEverAffectNavigation(false);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetRelativeScale3D(FVector(0.7f));
    MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderAsset.Succeeded())
    {
        MeshComp->SetStaticMesh(CylinderAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMatFinder.Succeeded())
    {
        MeshComp->SetMaterial(0, BaseMatFinder.Object);
    }

    AttributeComp = CreateDefaultSubobject<UB3DAttributeComponent>(TEXT("AttributeComp"));

    Tags.Add(TEXT("Enemy"));
}

void AB3DEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    ConfigureEnemy(EnemyType, 1.0f);
}

void AB3DEnemyBase::ConfigureEnemy(EB3DEnemyType InType, float HealthMultiplier)
{
    EnemyType = InType;

    switch (EnemyType)
    {
    case EB3DEnemyType::Normal:
        AttributeComp->Attributes.MaxHP = 25.0f * HealthMultiplier;
        AttributeComp->Attributes.CurrentHP = AttributeComp->Attributes.MaxHP;
        MoveSpeed = 220.0f;
        ContactDamage = 6.0f;
        DropMaterialCount = 1;
        SetActorScale3D(FVector(1.0f));
        break;

    case EB3DEnemyType::Dasher:
        AttributeComp->Attributes.MaxHP = 15.0f * HealthMultiplier;
        AttributeComp->Attributes.CurrentHP = AttributeComp->Attributes.MaxHP;
        MoveSpeed = 340.0f;
        ContactDamage = 10.0f;
        DropMaterialCount = 2;
        SetActorScale3D(FVector(0.85f));
        break;

    case EB3DEnemyType::Boss:
        AttributeComp->Attributes.MaxHP = 450.0f * HealthMultiplier;
        AttributeComp->Attributes.CurrentHP = AttributeComp->Attributes.MaxHP;
        MoveSpeed = 135.0f;
        ContactDamage = 20.0f;
        DropMaterialCount = 12;
        SetActorScale3D(FVector(2.3f));
        break;
    }

    if (UMaterialInstanceDynamic* EnemyMat = MeshComp->CreateDynamicMaterialInstance(0))
    {
        FLinearColor Color = (EnemyType == EB3DEnemyType::Normal) ? FLinearColor(0.92f, 0.16f, 0.16f, 1.0f) :
                             (EnemyType == EB3DEnemyType::Dasher) ? FLinearColor(1.0f, 0.45f, 0.05f, 1.0f) :
                             FLinearColor(0.65f, 0.10f, 0.85f, 1.0f);
        EnemyMat->SetVectorParameterValue(TEXT("Color"), Color);
    }
}

void AB3DEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveTowardsPlayer(DeltaTime);
    HandleContactDamage();

    if (ContactTimer > 0.0f)
    {
        ContactTimer -= DeltaTime;
    }

    if (HitFlashTimer > 0.0f)
    {
        HitFlashTimer -= DeltaTime;
    }
}

void AB3DEnemyBase::MoveTowardsPlayer(float DeltaTime)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn)
    {
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = PlayerPawn->GetActorLocation();
    FVector ToPlayer = TargetLoc - MyLoc;
    ToPlayer.Z = 0.0f;
    const float Dist = ToPlayer.Size();

    if (EnemyType == EB3DEnemyType::Dasher)
    {
        if (bCharging)
        {
            ChargeTimer -= DeltaTime;
            FHitResult Hit;
            AddActorWorldOffset(ChargeDirection * 750.0f * DeltaTime, true, &Hit);
            FVector Loc = GetActorLocation();
            Loc.Z = 45.0f;
            SetActorLocation(Loc);
            if (ChargeTimer <= 0.0f)
            {
                bCharging = false;
            }
            return;
        }
        else if (Dist < 420.0f && FMath::FRand() < 0.02f)
        {
            // Start charge
            bCharging = true;
            ChargeTimer = 0.65f;
            ChargeDirection = ToPlayer.GetSafeNormal();
            SetActorRotation(ChargeDirection.Rotation());
            return;
        }
    }

    if (Dist > 40.0f)
    {
        ToPlayer.Normalize();
        SetActorRotation(ToPlayer.Rotation());
        FHitResult Hit;
        AddActorWorldOffset(ToPlayer * MoveSpeed * DeltaTime, true, &Hit);
        FVector Loc = GetActorLocation();
        Loc.Z = 45.0f;
        SetActorLocation(Loc);
    }
}

void AB3DEnemyBase::HandleContactDamage()
{
    if (ContactTimer > 0.0f)
    {
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn)
    {
        return;
    }

    const float DistSq = FVector::DistSquared2D(GetActorLocation(), PlayerPawn->GetActorLocation());
    const float TouchRadius = 65.0f * GetActorScale3D().X;

    if (DistSq < FMath::Square(TouchRadius))
    {
        FDamageEvent DamageEvent;
        PlayerPawn->TakeDamage(ContactDamage, DamageEvent, GetController(), this);
        ContactTimer = 0.85f; // Cooldown between contact hits
    }
}

float AB3DEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (AttributeComp && AttributeComp->IsAlive())
    {
        AttributeComp->ApplyDamage(ActualDamage, DamageCauser, false);
        HitFlashTimer = 0.08f;

        if (!AttributeComp->IsAlive())
        {
            Die();
        }
    }
    return ActualDamage;
}

void AB3DEnemyBase::Die()
{
    OnEnemyDied.Broadcast(this);

    // Spawn materials
    UWorld* World = GetWorld();
    if (World)
    {
        for (int32 i = 0; i < DropMaterialCount; ++i)
        {
            FVector Offset = FVector(FMath::FRandRange(-30.0f, 30.0f), FMath::FRandRange(-30.0f, 30.0f), 10.0f);
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            World->SpawnActor<AB3DDropMaterial>(AB3DDropMaterial::StaticClass(), GetActorLocation() + Offset, FRotator::ZeroRotator, SpawnParams);
        }
    }

    Destroy();
}
