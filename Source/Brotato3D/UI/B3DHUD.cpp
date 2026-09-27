#include "UI/B3DHUD.h"
#include "Character/B3DCharacter.h"
#include "Core/B3DAttributeComponent.h"
#include "Core/B3DGameMode.h"
#include "Engine/Canvas.h"

AB3DHUD::AB3DHUD()
{
}

void AB3DHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas)
    {
        return;
    }

    const float ScreenW = Canvas->ClipX;
    const float ScreenH = Canvas->ClipY;

    DrawTopWaveTimer(ScreenW, ScreenH);
    DrawHealthBar(ScreenW, ScreenH);
    DrawMaterialCounter(ScreenW, ScreenH);
    DrawWeaponStatus(ScreenW, ScreenH);
    DrawBannerNotice(ScreenW, ScreenH);
}

void AB3DHUD::DrawTopWaveTimer(float ScreenWidth, float ScreenHeight)
{
    AB3DGameMode* GM = Cast<AB3DGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM)
    {
        return;
    }

    const int32 WaveNum = GM->GetCurrentWave();
    const float TimeLeft = GM->GetRemainingTime();
    const int32 Seconds = FMath::Clamp(FMath::CeilToInt(TimeLeft), 0, 9999);
    const int32 EnemiesAlive = GM->GetAliveEnemyCount();

    // Top Header Background
    const float HeaderW = 420.0f;
    const float HeaderH = 70.0f;
    const float HeaderX = (ScreenWidth - HeaderW) * 0.5f;
    const float HeaderY = 15.0f;

    DrawRect(FLinearColor(0.05f, 0.05f, 0.08f, 0.75f), HeaderX, HeaderY, HeaderW, HeaderH);
    DrawRect(FLinearColor(0.85f, 0.65f, 0.15f, 1.0f), HeaderX, HeaderY + HeaderH - 3.0f, HeaderW, 3.0f);

    FString TimerStr = FString::Printf(TEXT("WAVE %d   |   TIME: %02d:%02d"), WaveNum, Seconds / 60, Seconds % 60);
    DrawText(TimerStr, FLinearColor(1.0f, 0.95f, 0.4f, 1.0f), HeaderX + 45.0f, HeaderY + 12.0f, nullptr, 1.35f);

    FString EnemiesStr = FString::Printf(TEXT("Enemies in Arena: %d"), EnemiesAlive);
    DrawText(EnemiesStr, FLinearColor(0.8f, 0.8f, 0.8f, 1.0f), HeaderX + 110.0f, HeaderY + 42.0f, nullptr, 1.0f);
}

void AB3DHUD::DrawHealthBar(float ScreenWidth, float ScreenHeight)
{
    APawn* PlayerPawn = GetOwningPawn();
    if (!PlayerPawn)
    {
        return;
    }

    UB3DAttributeComponent* Attrib = PlayerPawn->FindComponentByClass<UB3DAttributeComponent>();
    if (!Attrib)
    {
        return;
    }

    const float CurrentHP = Attrib->Attributes.CurrentHP;
    const float MaxHP = Attrib->Attributes.MaxHP;
    const float Pct = Attrib->GetHealthPercent();

    const float BarW = 340.0f;
    const float BarH = 24.0f;
    const float BarX = (ScreenWidth - BarW) * 0.5f;
    const float BarY = ScreenHeight - 75.0f;

    // Background
    DrawRect(FLinearColor(0.08f, 0.08f, 0.08f, 0.85f), BarX - 3.0f, BarY - 3.0f, BarW + 6.0f, BarH + 6.0f);
    DrawRect(HealthBarBgColor, BarX, BarY, BarW, BarH);

    // Fill
    const float FillW = BarW * FMath::Clamp(Pct, 0.0f, 1.0f);
    DrawRect(HealthBarFillColor, BarX, BarY, FillW, BarH);

    // Text
    FString HPStr = FString::Printf(TEXT("HP: %d / %d"), FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP));
    DrawText(HPStr, FLinearColor::White, BarX + (BarW * 0.5f) - 40.0f, BarY + 4.0f, nullptr, 1.0f);
}

void AB3DHUD::DrawMaterialCounter(float ScreenWidth, float ScreenHeight)
{
    APawn* PlayerPawn = GetOwningPawn();
    if (!PlayerPawn)
    {
        return;
    }

    UB3DAttributeComponent* Attrib = PlayerPawn->FindComponentByClass<UB3DAttributeComponent>();
    if (!Attrib)
    {
        return;
    }

    const int32 Materials = Attrib->Attributes.Materials;
    const float BoxX = 30.0f;
    const float BoxY = 30.0f;
    const float BoxW = 210.0f;
    const float BoxH = 60.0f;

    DrawRect(FLinearColor(0.05f, 0.05f, 0.08f, 0.75f), BoxX, BoxY, BoxW, BoxH);
    DrawRect(FLinearColor(0.2f, 0.8f, 0.3f, 1.0f), BoxX, BoxY, 4.0f, BoxH);

    FString MatStr = FString::Printf(TEXT("XP / Crystals: %d"), Materials);
    DrawText(MatStr, FLinearColor(0.3f, 1.0f, 0.4f, 1.0f), BoxX + 16.0f, BoxY + 18.0f, nullptr, 1.25f);
}

void AB3DHUD::DrawWeaponStatus(float ScreenWidth, float ScreenHeight)
{
    AB3DCharacter* B3DChar = Cast<AB3DCharacter>(GetOwningPawn());
    if (!B3DChar)
    {
        return;
    }

    const int32 Count = B3DChar->GetEquippedWeaponCount();
    const float BoxX = ScreenWidth - 240.0f;
    const float BoxY = 30.0f;
    const float BoxW = 210.0f;
    const float BoxH = 60.0f;

    DrawRect(FLinearColor(0.05f, 0.05f, 0.08f, 0.75f), BoxX, BoxY, BoxW, BoxH);
    DrawRect(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f), BoxX + BoxW - 4.0f, BoxY, 4.0f, BoxH);

    FString WeaponStr = FString::Printf(TEXT("Weapons: %d / 6 Slots"), Count);
    DrawText(WeaponStr, FLinearColor(0.4f, 0.8f, 1.0f, 1.0f), BoxX + 16.0f, BoxY + 18.0f, nullptr, 1.2f);
}

void AB3DHUD::DrawBannerNotice(float ScreenWidth, float ScreenHeight)
{
    AB3DGameMode* GM = Cast<AB3DGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM)
    {
        return;
    }

    if (GM->IsWaveCompleted())
    {
        const float BannerW = 460.0f;
        const float BannerH = 65.0f;
        const float BannerX = (ScreenWidth - BannerW) * 0.5f;
        const float BannerY = (ScreenHeight - BannerH) * 0.5f;

        DrawRect(FLinearColor(0.02f, 0.35f, 0.08f, 0.85f), BannerX, BannerY, BannerW, BannerH);
        DrawText(TEXT("WAVE 1 SURVIVED! VICTORY!"), FLinearColor(0.4f, 1.0f, 0.4f, 1.0f), BannerX + 35.0f, BannerY + 18.0f, nullptr, 1.5f);
    }
    else if (GM->IsGameOver())
    {
        const float BannerW = 460.0f;
        const float BannerH = 65.0f;
        const float BannerX = (ScreenWidth - BannerW) * 0.5f;
        const float BannerY = (ScreenHeight - BannerH) * 0.5f;

        DrawRect(FLinearColor(0.45f, 0.05f, 0.05f, 0.85f), BannerX, BannerY, BannerW, BannerH);
        DrawText(TEXT("POTATO OVERWHELMED! DEFEAT!"), FLinearColor(1.0f, 0.3f, 0.3f, 1.0f), BannerX + 25.0f, BannerY + 18.0f, nullptr, 1.5f);
    }
}
