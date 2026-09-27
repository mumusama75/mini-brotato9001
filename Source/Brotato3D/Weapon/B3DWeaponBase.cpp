#include "Weapon/B3DWeaponBase.h"
#include "Weapon/B3DProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AB3DWeaponBase::AB3DWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetCanEverAffectNavigation(false);
    WeaponMesh->SetRelativeScale3D(FVector(0.5f, 0.16f, 0.16f));

    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(WeaponMesh);
    MuzzleLocation->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeAsset.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CubeAsset.Object);
    }

    ProjectileClass = AB3DProjectile::StaticClass();
    ConfigureWeaponType(EB3DWeaponType::Pistol, 1);
}

void AB3DWeaponBase::BeginPlay()
{
    Super::BeginPlay();

    UMaterialInterface* BaseMat = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")));
    if (BaseMat && WeaponMesh)
    {
        UMaterialInstanceDynamic* WepMat = UMaterialInstanceDynamic::Create(BaseMat, this);
        WepMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.24f, 0.30f, 0.40f, 1.0f));
        WeaponMesh->SetMaterial(0, WepMat);
    }

    CooldownTimer = FMath::FRandRange(0.0f, 0.2f); // Desynchronize multi-weapons
}

void AB3DWeaponBase::ConfigureWeaponType(EB3DWeaponType NewType, int32 NewTier)
{
    WeaponType = NewType;
    Tier = FMath::Clamp(NewTier, 1, 4);

    const float TierMult = 1.0f + (Tier - 1) * 0.35f;

    switch (WeaponType)
    {
    case EB3DWeaponType::Pistol:
        BaseDamage = 14.0f * TierMult;
        FireCooldown = 0.55f / FMath::Sqrt(TierMult);
        AttackRange = 750.0f;
        ProjectilesPerShot = 1;
        SpreadHalfAngle = 0.0f;
        PierceCount = 0;
        HomingStrength = 0.0f;
        WeaponMesh->SetRelativeScale3D(FVector(0.42f, 0.14f, 0.14f));
        break;

    case EB3DWeaponType::Shotgun:
        BaseDamage = 8.0f * TierMult;
        FireCooldown = 1.15f / FMath::Sqrt(TierMult);
        AttackRange = 550.0f;
        ProjectilesPerShot = 4 + Tier;
        SpreadHalfAngle = 18.0f;
        PierceCount = 0;
        HomingStrength = 0.0f;
        WeaponMesh->SetRelativeScale3D(FVector(0.65f, 0.22f, 0.16f));
        break;

    case EB3DWeaponType::Rifle:
        BaseDamage = 22.0f * TierMult;
        FireCooldown = 0.85f / FMath::Sqrt(TierMult);
        AttackRange = 900.0f;
        ProjectilesPerShot = 1;
        SpreadHalfAngle = 0.0f;
        PierceCount = 1 + Tier;
        HomingStrength = 0.0f;
        WeaponMesh->SetRelativeScale3D(FVector(0.75f, 0.12f, 0.12f));
        break;

    case EB3DWeaponType::Launcher:
        BaseDamage = 35.0f * TierMult;
        FireCooldown = 1.45f / FMath::Sqrt(TierMult);
        AttackRange = 800.0f;
        ProjectilesPerShot = 1;
        SpreadHalfAngle = 0.0f;
        PierceCount = 0;
        HomingStrength = 0.8f;
        WeaponMesh->SetRelativeScale3D(FVector(0.60f, 0.25f, 0.25f));
        break;
    }
}

AActor* AB3DWeaponBase::FindBestTarget()
{
    AActor* BestTarget = nullptr;
    float MinDistSq = FMath::Square(AttackRange);
    const FVector MyLoc = GetActorLocation();

    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Candidate = *It;
        if (Candidate && Candidate->ActorHasTag(TEXT("Enemy")))
        {
            const float DistSq = FVector::DistSquared2D(MyLoc, Candidate->GetActorLocation());
            if (DistSq < MinDistSq)
            {
                MinDistSq = DistSq;
                BestTarget = Candidate;
            }
        }
    }
    return BestTarget;
}

void AB3DWeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Target finding
    AActor* Target = FindBestTarget();
    CurrentTarget = Target;

    if (Target)
    {
        FVector Dir = Target->GetActorLocation() - WeaponMesh->GetComponentLocation();
        Dir.Z = 0.0f;
        if (!Dir.IsNearlyZero())
        {
            FRotator TargetRot = Dir.Rotation();
            WeaponMesh->SetWorldRotation(FMath::RInterpTo(WeaponMesh->GetComponentRotation(), TargetRot, DeltaTime, 14.0f));
        }
    }

    // Recoil recovery
    RecoilOffset = FMath::FInterpTo(RecoilOffset, 0.0f, DeltaTime, 18.0f);
    WeaponMesh->SetRelativeLocation(FVector(RecoilOffset, 0.0f, 0.0f));

    // Fire timer
    if (CooldownTimer > 0.0f)
    {
        CooldownTimer -= DeltaTime;
    }

    if (CooldownTimer <= 0.0f && Target)
    {
        Fire(Target);
        CooldownTimer = FireCooldown;
    }
}

void AB3DWeaponBase::Fire(AActor* Target)
{
    if (!Target || !ProjectileClass)
    {
        return;
    }

    FVector MuzzlePos = MuzzleLocation->GetComponentLocation();
    FVector BaseAimDir = (Target->GetActorLocation() - MuzzlePos);
    BaseAimDir.Z = 0.0f;
    BaseAimDir.Normalize();

    const bool bCrit = FMath::FRand() < 0.12f;
    const float FinalDmg = BaseDamage * (bCrit ? 1.5f : 1.0f);

    for (int32 i = 0; i < ProjectilesPerShot; ++i)
    {
        FVector ShotDir = BaseAimDir;
        if (SpreadHalfAngle > 0.0f && ProjectilesPerShot > 1)
        {
            float AngleOffset = FMath::FRandRange(-SpreadHalfAngle, SpreadHalfAngle);
            ShotDir = FRotator(0.0f, AngleOffset, 0.0f).RotateVector(BaseAimDir);
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = Cast<APawn>(GetOwner());
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AB3DProjectile* Proj = GetWorld()->SpawnActor<AB3DProjectile>(ProjectileClass, MuzzlePos, ShotDir.Rotation(), SpawnParams);
        if (Proj)
        {
            Proj->Initialize(ShotDir, 1800.0f, FinalDmg, bCrit, PierceCount, BounceCount, HomingStrength, Target);
        }
    }

    // Apply recoil impulse
    RecoilOffset = -14.0f;
}
