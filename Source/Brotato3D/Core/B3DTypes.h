#pragma once

#include "CoreMinimal.h"
#include "B3DTypes.generated.h"

UENUM(BlueprintType)
enum class EB3DWeaponType : uint8
{
    Pistol UMETA(DisplayName = "Pistol"),
    Shotgun UMETA(DisplayName = "Shotgun"),
    Rifle UMETA(DisplayName = "Rifle"),
    Launcher UMETA(DisplayName = "Launcher")
};

UENUM(BlueprintType)
enum class EB3DEnemyType : uint8
{
    Normal UMETA(DisplayName = "Normal Walker"),
    Dasher UMETA(DisplayName = "Dasher Charger"),
    Boss UMETA(DisplayName = "Arena Boss")
};

USTRUCT(BlueprintType)
struct FB3DAttributes
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHP = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float CurrentHP = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed = 480.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float MeleeDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float RangedDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackSpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CritChance = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CritDamageMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    float Armor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    float DodgeChance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float LifeSteal = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float PickupRadius = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float Harvesting = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Materials = 0;
};
