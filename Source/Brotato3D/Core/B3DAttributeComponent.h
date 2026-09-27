#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "B3DTypes.h"
#include "B3DAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, float, CurrentHP, float, MaxHP, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialsChangedSignature, int32, TotalMaterials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BROTATO3D_API UB3DAttributeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UB3DAttributeComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    FB3DAttributes Attributes;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMaterialsChangedSignature OnMaterialsChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnDeathSignature OnDeath;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool ApplyDamage(float RawDamage, AActor* DamageCauser, bool bCanDodge = true);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void AddMaterials(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ProcessLifeSteal(float DamageDealt);

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAlive() const { return Attributes.CurrentHP > 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const { return Attributes.MaxHP > 0.0f ? Attributes.CurrentHP / Attributes.MaxHP : 0.0f; }
};
