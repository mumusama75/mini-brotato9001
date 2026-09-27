#include "Character/B3DCharacter.h"
#include "Core/B3DAttributeComponent.h"
#include "Weapon/B3DWeaponBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

AB3DCharacter::AB3DCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Capsule setup
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 50.0f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    // Don't rotate character when controller rotates
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Movement settings
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));
    GetCharacterMovement()->MaxWalkSpeed = 480.0f;

    // Camera Boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->TargetArmLength = 1350.0f;
    CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;

    // Top-down Camera
    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCamera->bUsePawnControlRotation = false;

    // Hero Mesh (Potato shape)
    HeroMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroMesh"));
    HeroMesh->SetupAttachment(RootComponent);
    HeroMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HeroMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
    HeroMesh->SetRelativeScale3D(FVector(0.85f, 0.85f, 1.05f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereAsset.Succeeded())
    {
        HeroMesh->SetStaticMesh(SphereAsset.Object);
    }

    // Weapon Orbit Root
    WeaponOrbitRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponOrbitRoot"));
    WeaponOrbitRoot->SetupAttachment(RootComponent);
    WeaponOrbitRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));

    // Initialize 6 socket components
    InitializeWeaponSockets();

    // Attributes
    AttributeComp = CreateDefaultSubobject<UB3DAttributeComponent>(TEXT("AttributeComp"));

    EquippedWeapons.Init(nullptr, 6);

    Tags.Add(TEXT("Player"));
}

void AB3DCharacter::InitializeWeaponSockets()
{
    WeaponSockets.Empty();
    for (int32 i = 0; i < 6; ++i)
    {
        FString SocketName = FString::Printf(TEXT("WeaponSocket_%d"), i);
        USceneComponent* SocketComp = CreateDefaultSubobject<USceneComponent>(*SocketName);
        SocketComp->SetupAttachment(WeaponOrbitRoot);

        const float AngleDeg = i * 60.0f;
        const float AngleRad = FMath::DegreesToRadians(AngleDeg);
        const float X = OrbitRadius * FMath::Cos(AngleRad);
        const float Y = OrbitRadius * FMath::Sin(AngleRad);

        SocketComp->SetRelativeLocation(FVector(X, Y, 0.0f));
        SocketComp->SetRelativeRotation(FRotator(0.0f, AngleDeg, 0.0f));
        WeaponSockets.Add(SocketComp);
    }
}

void AB3DCharacter::BeginPlay()
{
    Super::BeginPlay();

    UMaterialInterface* BaseMat = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")));
    if (BaseMat && HeroMesh)
    {
        UMaterialInstanceDynamic* HeroMat = UMaterialInstanceDynamic::Create(BaseMat, this);
        HeroMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.88f, 0.70f, 0.40f, 1.0f));
        HeroMesh->SetMaterial(0, HeroMat);
    }

    if (AttributeComp)
    {
        GetCharacterMovement()->MaxWalkSpeed = AttributeComp->Attributes.MoveSpeed;
        AttributeComp->OnDeath.AddDynamic(this, &AB3DCharacter::HandleDeath);
    }

    SpawnStarterLoadout();
}

void AB3DCharacter::SpawnStarterLoadout()
{
    // Equip initial 6 weapons across the sockets
    EquipWeapon(EB3DWeaponType::Pistol, 0, 1);
    EquipWeapon(EB3DWeaponType::Shotgun, 1, 1);
    EquipWeapon(EB3DWeaponType::Rifle, 2, 1);
    EquipWeapon(EB3DWeaponType::Launcher, 3, 1);
    EquipWeapon(EB3DWeaponType::Pistol, 4, 1);
    EquipWeapon(EB3DWeaponType::Shotgun, 5, 1);
}

void AB3DCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Continuous weapon orbit rotation
    if (WeaponOrbitRoot)
    {
        WeaponOrbitRoot->AddLocalRotation(FRotator(0.0f, OrbitSpeed * DeltaTime, 0.0f));
    }
}

void AB3DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AB3DCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AB3DCharacter::MoveRight);
}

void AB3DCharacter::MoveForward(float Value)
{
    if (Value != 0.0f && Controller)
    {
        // Top-down fixed axes: Forward is world +X
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
    }
}

void AB3DCharacter::MoveRight(float Value)
{
    if (Value != 0.0f && Controller)
    {
        // Top-down fixed axes: Right is world +Y
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
}

bool AB3DCharacter::EquipWeapon(EB3DWeaponType WeaponType, int32 SocketIndex, int32 Tier)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    // Find available socket if not specified
    if (SocketIndex < 0 || SocketIndex >= 6)
    {
        SocketIndex = -1;
        for (int32 i = 0; i < EquippedWeapons.Num(); ++i)
        {
            if (EquippedWeapons[i] == nullptr)
            {
                SocketIndex = i;
                break;
            }
        }
    }

    if (SocketIndex < 0 || SocketIndex >= 6 || SocketIndex >= WeaponSockets.Num())
    {
        return false; // No available slot
    }

    // Remove existing weapon in slot if any
    if (EquippedWeapons[SocketIndex])
    {
        EquippedWeapons[SocketIndex]->Destroy();
        EquippedWeapons[SocketIndex] = nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AB3DWeaponBase* NewWeapon = World->SpawnActor<AB3DWeaponBase>(AB3DWeaponBase::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (NewWeapon)
    {
        NewWeapon->AttachToComponent(WeaponSockets[SocketIndex], FAttachmentTransformRules::SnapToTargetIncludingScale);
        NewWeapon->ConfigureWeaponType(WeaponType, Tier);
        EquippedWeapons[SocketIndex] = NewWeapon;
        return true;
    }

    return false;
}

int32 AB3DCharacter::GetEquippedWeaponCount() const
{
    int32 Count = 0;
    for (const AB3DWeaponBase* W : EquippedWeapons)
    {
        if (W != nullptr)
        {
            Count++;
        }
    }
    return Count;
}

void AB3DCharacter::RemoveAllWeapons()
{
    for (int32 i = 0; i < EquippedWeapons.Num(); ++i)
    {
        if (EquippedWeapons[i])
        {
            EquippedWeapons[i]->Destroy();
            EquippedWeapons[i] = nullptr;
        }
    }
}

float AB3DCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (AttributeComp && AttributeComp->IsAlive())
    {
        AttributeComp->ApplyDamage(ActualDamage, DamageCauser, true);
    }

    return ActualDamage;
}

void AB3DCharacter::HandleDeath(AActor* DeadActor)
{
    // Disable inputs and movement on defeat
    DisableInput(Cast<APlayerController>(GetController()));
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
