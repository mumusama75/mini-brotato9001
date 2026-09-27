#include "Core/B3DGameMode.h"
#include "Character/B3DCharacter.h"
#include "Enemy/B3DEnemyBase.h"
#include "UI/B3DHUD.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealClient.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformMisc.h"

AB3DGameMode::AB3DGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultPawnClass = AB3DCharacter::StaticClass();
    HUDClass = AB3DHUD::StaticClass();
}

void AB3DGameMode::BeginPlay()
{
    Super::BeginPlay();

    BuildProceduralArena();

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
    {
        PlayerPawn->SetActorLocation(FVector(0.0f, 0.0f, 60.0f));
    }

    StartWave(1);

    bIsQARun = FParse::Param(FCommandLine::Get(), TEXT("B3DQA"));
    if (bIsQARun)
    {
        UE_LOG(LogTemp, Display, TEXT("B3D_QA: Automated QA Mode Initialized successfully."));
    }
}

void AB3DGameMode::BuildProceduralArena()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Load Basic Cube
    UStaticMesh* CubeMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
    if (!CubeMesh)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Base material for color tinting
    UMaterialInterface* BaseMat = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")));

    // Floor (40m x 40m)
    AStaticMeshActor* FloorActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, 0.0f, -5.0f), FRotator::ZeroRotator, SpawnParams);
    if (FloorActor && FloorActor->GetStaticMeshComponent())
    {
        FloorActor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
        FloorActor->GetStaticMeshComponent()->SetRelativeScale3D(FVector((ArenaHalfSize * 2.0f) / 100.0f, (ArenaHalfSize * 2.0f) / 100.0f, 0.1f));
        FloorActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));

        if (BaseMat)
        {
            UMaterialInstanceDynamic* FloorMat = UMaterialInstanceDynamic::Create(BaseMat, FloorActor);
            FloorMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.14f, 0.16f, 0.20f, 1.0f));
            FloorActor->GetStaticMeshComponent()->SetMaterial(0, FloorMat);
        }
    }

    // Perimeter boundary walls
    const float WallHeight = 120.0f;
    const float WallScaleZ = WallHeight / 100.0f;
    const float WallThickness = 1.0f;
    const float WallSpan = (ArenaHalfSize * 2.0f) / 100.0f;

    auto ApplyWallStyle = [BaseMat, CubeMesh](AStaticMeshActor* Wall)
    {
        if (Wall && Wall->GetStaticMeshComponent())
        {
            Wall->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
            Wall->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
            if (BaseMat)
            {
                UMaterialInstanceDynamic* WallMat = UMaterialInstanceDynamic::Create(BaseMat, Wall);
                WallMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.08f, 0.09f, 0.12f, 1.0f));
                Wall->GetStaticMeshComponent()->SetMaterial(0, WallMat);
            }
        }
    };

    // North Wall
    AStaticMeshActor* NorthWall = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, ArenaHalfSize, WallHeight * 0.5f), FRotator::ZeroRotator, SpawnParams);
    if (NorthWall && NorthWall->GetStaticMeshComponent())
    {
        NorthWall->GetStaticMeshComponent()->SetRelativeScale3D(FVector(WallSpan, WallThickness, WallScaleZ));
        ApplyWallStyle(NorthWall);
    }

    // South Wall
    AStaticMeshActor* SouthWall = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, -ArenaHalfSize, WallHeight * 0.5f), FRotator::ZeroRotator, SpawnParams);
    if (SouthWall && SouthWall->GetStaticMeshComponent())
    {
        SouthWall->GetStaticMeshComponent()->SetRelativeScale3D(FVector(WallSpan, WallThickness, WallScaleZ));
        ApplyWallStyle(SouthWall);
    }

    // East Wall
    AStaticMeshActor* EastWall = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(ArenaHalfSize, 0.0f, WallHeight * 0.5f), FRotator::ZeroRotator, SpawnParams);
    if (EastWall && EastWall->GetStaticMeshComponent())
    {
        EastWall->GetStaticMeshComponent()->SetRelativeScale3D(FVector(WallThickness, WallSpan, WallScaleZ));
        ApplyWallStyle(EastWall);
    }

    // West Wall
    AStaticMeshActor* WestWall = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(-ArenaHalfSize, 0.0f, WallHeight * 0.5f), FRotator::ZeroRotator, SpawnParams);
    if (WestWall && WestWall->GetStaticMeshComponent())
    {
        WestWall->GetStaticMeshComponent()->SetRelativeScale3D(FVector(WallThickness, WallSpan, WallScaleZ));
        ApplyWallStyle(WestWall);
    }

    // Setup Directional Sunlight (Movable dynamic lighting)
    ADirectionalLight* SunLight = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.0f, 0.0f, 800.0f), FRotator(-55.0f, -40.0f, 0.0f), SpawnParams);
    if (SunLight && SunLight->GetLightComponent())
    {
        SunLight->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        SunLight->GetLightComponent()->SetIntensity(3.5f);
        SunLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.96f, 0.88f));
    }

    // Setup Skylight (Movable dynamic lighting)
    ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector(0.0f, 0.0f, 900.0f), FRotator::ZeroRotator, SpawnParams);
    if (Sky && Sky->GetLightComponent())
    {
        Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        Sky->GetLightComponent()->SetIntensity(1.2f);
        Sky->GetLightComponent()->SetLightColor(FLinearColor(0.8f, 0.88f, 1.0f));
    }
}

void AB3DGameMode::StartWave(int32 WaveNum)
{
    CurrentWave = WaveNum;
    RemainingWaveTime = WaveDuration;
    bWaveActive = true;
    bWaveCompleted = false;
    bGameOver = false;
    SpawnTimer = 0.0f;

    OnWaveStarted.Broadcast(CurrentWave);
}

void AB3DGameMode::EndWave(bool bVictory)
{
    bWaveActive = false;
    bWaveCompleted = bVictory;
    bGameOver = !bVictory;

    OnWaveEnded.Broadcast(bVictory);
}

void AB3DGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bWaveActive)
    {
        RemainingWaveTime -= DeltaTime;

        SpawnTimer += DeltaTime;
        if (SpawnTimer >= SpawnInterval)
        {
            SpawnTimer = 0.0f;
            SpawnWaveEnemies();
        }

        if (RemainingWaveTime <= 0.0f)
        {
            RemainingWaveTime = 0.0f;
            EndWave(true);
        }
    }

    if (bIsQARun)
    {
        QATimer += DeltaTime;

        // Simulate top-down player movement during QA to show off kiting
        if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
        {
            const float MoveAngle = QATimer * 1.5f;
            FVector MoveDir(FMath::Cos(MoveAngle), FMath::Sin(MoveAngle), 0.0f);
            PlayerPawn->AddMovementInput(MoveDir, 0.75f);
        }

        if (QAScreenshotStep == 0 && QATimer >= 3.0f)
        {
            QAScreenshotStep++;
            FString ShotPath = FPaths::ProjectSavedDir() / TEXT("QA/Brotato3D_QA_WaveCombat_Early.png");
            FScreenshotRequest::RequestScreenshot(ShotPath, true, false);
            UE_LOG(LogTemp, Display, TEXT("B3D_QA: Capture Step 1 (Early Combat) -> %s (Alive=%d)"), *ShotPath, ActiveEnemies.Num());
        }
        else if (QAScreenshotStep == 1 && QATimer >= 7.5f)
        {
            QAScreenshotStep++;
            FString ShotPath = FPaths::ProjectSavedDir() / TEXT("QA/Brotato3D_QA_WaveCombat_Mid.png");
            FScreenshotRequest::RequestScreenshot(ShotPath, true, false);
            UE_LOG(LogTemp, Display, TEXT("B3D_QA: Capture Step 2 (Mid Combat) -> %s (Alive=%d)"), *ShotPath, ActiveEnemies.Num());
        }
        else if (QAScreenshotStep == 2 && QATimer >= 13.0f)
        {
            QAScreenshotStep++;
            FString ShotPath = FPaths::ProjectSavedDir() / TEXT("QA/Brotato3D_QA_WaveCombat_Swarm.png");
            FScreenshotRequest::RequestScreenshot(ShotPath, true, false);
            UE_LOG(LogTemp, Display, TEXT("B3D_QA: Capture Step 3 (Swarm Peak) -> %s (Alive=%d)"), *ShotPath, ActiveEnemies.Num());
        }
        else if (QATimer >= 16.0f)
        {
            UE_LOG(LogTemp, Display, TEXT("B3D_QA: Completed 16s QA slice. Exiting cleanly."));
            FGenericPlatformMisc::RequestExit(false);
        }
    }
}

FVector AB3DGameMode::GetRandomSpawnPosition() const
{
    const float Margin = 150.0f;
    const float SpawnDist = ArenaHalfSize - Margin;

    // Pick a side: 0=North, 1=South, 2=East, 3=West
    int32 Side = FMath::RandRange(0, 3);
    FVector Pos = FVector::ZeroVector;

    switch (Side)
    {
    case 0: // North
        Pos = FVector(FMath::FRandRange(-SpawnDist, SpawnDist), SpawnDist, 45.0f);
        break;
    case 1: // South
        Pos = FVector(FMath::FRandRange(-SpawnDist, SpawnDist), -SpawnDist, 45.0f);
        break;
    case 2: // East
        Pos = FVector(SpawnDist, FMath::FRandRange(-SpawnDist, SpawnDist), 45.0f);
        break;
    case 3: // West
        Pos = FVector(-SpawnDist, FMath::FRandRange(-SpawnDist, SpawnDist), 45.0f);
        break;
    }

    return Pos;
}

void AB3DGameMode::SpawnWaveEnemies()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Clean stale pointers
    ActiveEnemies.RemoveAll([](const TWeakObjectPtr<AB3DEnemyBase>& Ptr) { return !Ptr.IsValid(); });

    // Spawn 2 to 4 enemies per wave pulse
    int32 CountToSpawn = FMath::RandRange(2, 4);

    for (int32 i = 0; i < CountToSpawn; ++i)
    {
        if (ActiveEnemies.Num() >= MaxSimultaneousEnemies)
        {
            break;
        }

        FVector SpawnPos = GetRandomSpawnPosition();
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AB3DEnemyBase* Enemy = World->SpawnActor<AB3DEnemyBase>(AB3DEnemyBase::StaticClass(), SpawnPos, FRotator::ZeroRotator, SpawnParams);
        if (Enemy)
        {
            // 75% Normal, 25% Dasher
            EB3DEnemyType TypeToSpawn = (FMath::FRand() < 0.75f) ? EB3DEnemyType::Normal : EB3DEnemyType::Dasher;
            const float HpMultiplier = 1.0f + (CurrentWave - 1) * 0.2f;
            Enemy->ConfigureEnemy(TypeToSpawn, HpMultiplier);

            Enemy->OnEnemyDied.AddDynamic(this, &AB3DGameMode::OnEnemyDestroyed);
            ActiveEnemies.Add(Enemy);
        }
    }
}

void AB3DGameMode::OnEnemyDestroyed(AB3DEnemyBase* DeadEnemy)
{
    ActiveEnemies.Remove(DeadEnemy);
}
