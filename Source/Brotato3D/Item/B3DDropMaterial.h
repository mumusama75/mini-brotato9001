#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "B3DDropMaterial.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BROTATO3D_API AB3DDropMaterial : public AActor
{
    GENERATED_BODY()

public:
    AB3DDropMaterial();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MaterialValue = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    float MagnetSpeed = 950.0f;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    bool bAttracting = false;
    TWeakObjectPtr<AActor> TargetPlayer;
};
