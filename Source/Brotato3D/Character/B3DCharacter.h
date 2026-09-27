#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/B3DTypes.h"
#include "B3DCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UB3DAttributeComponent;
class AB3DWeaponBase;

UCLASS()
class BROTATO3D_API AB3DCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AB3DCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* TopDownCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    UStaticMeshComponent* HeroMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    UB3DAttributeComponent* AttributeComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    USceneComponent* WeaponOrbitRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TArray<USceneComponent*> WeaponSockets;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TArray<AB3DWeaponBase*> EquippedWeapons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float OrbitSpeed = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float OrbitRadius = 95.0f;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool EquipWeapon(EB3DWeaponType WeaponType, int32 SocketIndex = -1, int32 Tier = 1);

    UFUNCTION(BlueprintPure, Category = "Combat")
    int32 GetEquippedWeaponCount() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RemoveAllWeapons();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void MoveForward(float Value);
    void MoveRight(float Value);

    UFUNCTION()
    void HandleDeath(AActor* DeadActor);

private:
    void InitializeWeaponSockets();
    void SpawnStarterLoadout();
};
