// Copyright (c) takumi2911 - BlackoutExfil
#include "GameMode/BXMatchGameMode.h"
#include "Systems/BXMatchSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABXMatchGameMode::ABXMatchGameMode()
{
    // デフォルト Pawn / HUD は Blueprint 子クラスで設定
}

void ABXMatchGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UBXMatchSubsystem* MS = GetMatchSubsystem())
    {
        MS->StartPreGame(PreGameCountdownSec);
    }

    if (RaidTimeLimitSec > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            RaidTimerHandle, this, &ABXMatchGameMode::OnRaidTimeUp,
            RaidTimeLimitSec, false);
    }
}

void ABXMatchGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(RaidTimerHandle);
    Super::EndPlay(EndPlayReason);
}

void ABXMatchGameMode::OnPlayerDied()
{
    if (UBXMatchSubsystem* MS = GetMatchSubsystem())
    {
        MS->EndMatch(false);
    }
}

void ABXMatchGameMode::OnPlayerReachedExtract(FName ExtractPointID)
{
    if (UBXMatchSubsystem* MS = GetMatchSubsystem())
    {
        MS->BeginExtraction(ExtractPointID);
    }
}

void ABXMatchGameMode::OnRaidTimeUp()
{
    UE_LOG(LogTemp, Warning, TEXT("[MatchGameMode] Raid time limit reached"));
    if (UBXMatchSubsystem* MS = GetMatchSubsystem())
    {
        MS->EndMatch(false);
    }
}

UBXMatchSubsystem* ABXMatchGameMode::GetMatchSubsystem() const
{
    if (UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UBXMatchSubsystem>();
    }
    return nullptr;
}
