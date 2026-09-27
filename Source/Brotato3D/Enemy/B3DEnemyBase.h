#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Core/B3DTypes.h"
#include "B3DEnemyBase.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UB3DAttributeComponent;
class AB3DDropMaterial;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDiedSignature, AB3DEnemyBase*, DeadEnemy);

UCLASS()
class BROTATO3D_API AB3DEnemyBase : public APawn
{
    GENERATED_BODY()

public:
    AB3DEnemyBase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UB3DAttributeComponent* AttributeComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    EB3DEnemyType EnemyType = EB3DEnemyType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float ContactDamage = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float MoveSpeed = 220.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 DropMaterialCount = 1;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDiedSignature OnEnemyDied;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ConfigureEnemy(EB3DEnemyType InType, float HealthMultiplier = 1.0f);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void MoveTowardsPlayer(float DeltaTime);
    void HandleContactDamage();
    void Die();

private:
    float ContactTimer = 0.0f;
    float HitFlashTimer = 0.0f;
    bool bCharging = false;
    float ChargeTimer = 0.0f;
    FVector ChargeDirection;
};
