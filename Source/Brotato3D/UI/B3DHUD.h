#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "B3DHUD.generated.h"

UCLASS()
class BROTATO3D_API AB3DHUD : public AHUD
{
    GENERATED_BODY()

public:
    AB3DHUD();

    virtual void DrawHUD() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    FLinearColor HealthBarFillColor = FLinearColor(0.1f, 0.85f, 0.2f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    FLinearColor HealthBarBgColor = FLinearColor(0.2f, 0.05f, 0.05f, 0.8f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    FLinearColor TextColor = FLinearColor::White;

private:
    void DrawTopWaveTimer(float ScreenWidth, float ScreenHeight);
    void DrawHealthBar(float ScreenWidth, float ScreenHeight);
    void DrawMaterialCounter(float ScreenWidth, float ScreenHeight);
    void DrawWeaponStatus(float ScreenWidth, float ScreenHeight);
    void DrawBannerNotice(float ScreenWidth, float ScreenHeight);
};
