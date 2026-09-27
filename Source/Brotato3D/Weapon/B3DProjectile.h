#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "B3DProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BROTATO3D_API AB3DProjectile : public AActor
{
    GENERATED_BODY()

public:
    AB3DProjectile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Damage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bIsCritical = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 PierceLeft = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 BouncesLeft = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HomingStrength = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Speed = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float KnockbackStrength = 350.0f;

    UPROPERTY()
    AActor* HomingTarget = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Initialize(FVector Direction, float InSpeed, float InDamage, bool bCrit, int32 InPierce = 0, int32 InBounce = 0, float InHoming = 0.0f, AActor* Target = nullptr);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    FVector MoveVelocity;
    TSet<TWeakObjectPtr<AActor>> HitActors;
    float LifeTime = 4.0f;
};
