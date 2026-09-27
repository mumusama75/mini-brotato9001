#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/B3DTypes.h"
#include "B3DGameMode.generated.h"

class AB3DEnemyBase;
class AB3DCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChangedSignature, int32, NewWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveEndedSignature, bool, bVictory);

UCLASS()
class BROTATO3D_API AB3DGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AB3DGameMode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 CurrentWave = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float WaveDuration = 30.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave")
    float RemainingWaveTime = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float SpawnInterval = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 MaxSimultaneousEnemies = 80;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
    float ArenaHalfSize = 1800.0f;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWaveChangedSignature OnWaveStarted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWaveEndedSignature OnWaveEnded;

    UFUNCTION(BlueprintPure, Category = "Wave")
    float GetRemainingTime() const { return RemainingWaveTime; }

    UFUNCTION(BlueprintPure, Category = "Wave")
    int32 GetCurrentWave() const { return CurrentWave; }

    UFUNCTION(BlueprintPure, Category = "Wave")
    int32 GetAliveEnemyCount() const { return ActiveEnemies.Num(); }

    UFUNCTION(BlueprintPure, Category = "Wave")
    bool IsWaveActive() const { return bWaveActive; }

    UFUNCTION(BlueprintPure, Category = "Wave")
    bool IsWaveCompleted() const { return bWaveCompleted; }

    UFUNCTION(BlueprintPure, Category = "Wave")
    bool IsGameOver() const { return bGameOver; }

    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave(int32 WaveNum);

    UFUNCTION(BlueprintCallable, Category = "Wave")
    void EndWave(bool bVictory);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SpawnWaveEnemies();
    void BuildProceduralArena();
    FVector GetRandomSpawnPosition() const;

    UFUNCTION()
    void OnEnemyDestroyed(AB3DEnemyBase* DeadEnemy);

private:
    bool bWaveActive = false;
    bool bWaveCompleted = false;
    bool bGameOver = false;

    float SpawnTimer = 0.0f;
    TArray<TWeakObjectPtr<AB3DEnemyBase>> ActiveEnemies;

    bool bIsQARun = false;
    float QATimer = 0.0f;
    int32 QAScreenshotStep = 0;
};
