#include "Core/B3DAttributeComponent.h"

UB3DAttributeComponent::UB3DAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UB3DAttributeComponent::ApplyDamage(float RawDamage, AActor* DamageCauser, bool bCanDodge)
{
    if (!IsAlive() || RawDamage <= 0.0f)
    {
        return false;
    }

    // Dodge check
    if (bCanDodge && Attributes.DodgeChance > 0.0f)
    {
        if (FMath::FRand() < FMath::Clamp(Attributes.DodgeChance, 0.0f, 0.60f)) // Cap at 60% dodge
        {
            // Dodged!
            return false;
        }
    }

    // Armor damage reduction: Effective damage = Raw / (1 + Armor * 0.066)
    float Mitigation = 1.0f;
    if (Attributes.Armor > 0.0f)
    {
        Mitigation = 1.0f / (1.0f + Attributes.Armor * 0.066f);
    }
    else if (Attributes.Armor < 0.0f)
    {
        // Negative armor increases damage taken
        Mitigation = 2.0f - (1.0f / (1.0f - Attributes.Armor * 0.066f));
    }

    const float FinalDamage = FMath::Max(1.0f, RawDamage * Mitigation);
    Attributes.CurrentHP = FMath::Clamp(Attributes.CurrentHP - FinalDamage, 0.0f, Attributes.MaxHP);

    OnHealthChanged.Broadcast(Attributes.CurrentHP, Attributes.MaxHP, -FinalDamage);

    if (Attributes.CurrentHP <= 0.0f)
    {
        OnDeath.Broadcast(GetOwner());
    }

    return true;
}

void UB3DAttributeComponent::Heal(float Amount)
{
    if (!IsAlive() || Amount <= 0.0f)
    {
        return;
    }

    const float OldHP = Attributes.CurrentHP;
    Attributes.CurrentHP = FMath::Clamp(Attributes.CurrentHP + Amount, 0.0f, Attributes.MaxHP);
    const float ActualHealed = Attributes.CurrentHP - OldHP;

    if (ActualHealed > 0.0f)
    {
        OnHealthChanged.Broadcast(Attributes.CurrentHP, Attributes.MaxHP, ActualHealed);
    }
}

void UB3DAttributeComponent::AddMaterials(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    Attributes.Materials += Amount;
    OnMaterialsChanged.Broadcast(Attributes.Materials);
}

void UB3DAttributeComponent::ProcessLifeSteal(float DamageDealt)
{
    if (Attributes.LifeSteal <= 0.0f || DamageDealt <= 0.0f)
    {
        return;
    }

    // Chance-based lifesteal or percentage
    if (FMath::FRand() < Attributes.LifeSteal)
    {
        Heal(1.0f); // Brotato style: 1 HP per lifesteal proc
    }
}
