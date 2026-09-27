#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/B3DTypes.h"
#include "B3DWeaponBase.generated.h"

class UStaticMeshComponent;
class AB3DProjectile;

UCLASS()
class BROTATO3D_API AB3DWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AB3DWeaponBase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* MuzzleLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    EB3DWeaponType WeaponType = EB3DWeaponType::Pistol;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 Tier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float BaseDamage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireCooldown = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 ProjectilesPerShot = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float SpreadHalfAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 PierceCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 BounceCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HomingStrength = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AB3DProjectile> ProjectileClass;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ConfigureWeaponType(EB3DWeaponType NewType, int32 NewTier = 1);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    AActor* FindBestTarget();
    void Fire(AActor* Target);

private:
    float CooldownTimer = 0.0f;
    float RecoilOffset = 0.0f;
    TWeakObjectPtr<AActor> CurrentTarget;
};
