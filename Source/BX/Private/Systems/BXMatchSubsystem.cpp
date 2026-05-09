// Copyright (c) takumi2911 - BlackoutExfil
#include "Systems/BXMatchSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void UBXMatchSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    MatchState = EBXMatchState::Idle;
}

void UBXMatchSubsystem::Deinitialize()
{
    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().ClearTimer(CountdownTimer);
        World->GetTimerManager().ClearTimer(ExtractTimer);
    }
    Super::Deinitialize();
}

void UBXMatchSubsystem::SetMatchState(EBXMatchState NewState)
{
    if (MatchState == NewState) { return; }
    const EBXMatchState Old = MatchState;
    MatchState = NewState;
    OnMatchStateChanged.Broadcast(Old, NewState);
    UE_LOG(LogTemp, Log, TEXT("[Match] State: %d -> %d"), (int)Old, (int)NewState);
}

void UBXMatchSubsystem::StartPreGame(int32 CountdownSecs)
{
    if (MatchState != EBXMatchState::Idle) { return; }
    SetMatchState(EBXMatchState::PreGame);
    CurrentResult = FBXRaidResult{};
    CountdownRemaining = FMath::Max(1, CountdownSecs);

    UWorld* World = GetGameInstance()->GetWorld();
    World->GetTimerManager().SetTimer(
        CountdownTimer, this, &UBXMatchSubsystem::TickCountdown,
        1.0f, true, 0.0f);
}

void UBXMatchSubsystem::TickCountdown()
{
    OnCountdownTick.Broadcast(CountdownRemaining);
    if (CountdownRemaining <= 0)
    {
        UWorld* World = GetGameInstance()->GetWorld();
        World->GetTimerManager().ClearTimer(CountdownTimer);
        StartMatch();
        return;
    }
    --CountdownRemaining;
}

void UBXMatchSubsystem::StartMatch()
{
    SetMatchState(EBXMatchState::InMatch);
    MatchStartTime = GetGameInstance()->GetWorld()->GetTimeSeconds();
    UE_LOG(LogTemp, Log, TEXT("[Match] RAID STARTED"));
}

void UBXMatchSubsystem::BeginExtraction(FName ExtractPointID)
{
    if (MatchState != EBXMatchState::InMatch) { return; }
    SetMatchState(EBXMatchState::Extracting);
    ActiveExtractID = ExtractPointID;
    ExtractionStart = GetGameInstance()->GetWorld()->GetTimeSeconds();

    const float ExtractionTimeSec = 5.0f;
    UWorld* World = GetGameInstance()->GetWorld();
    World->GetTimerManager().SetTimer(
        ExtractTimer, this, &UBXMatchSubsystem::OnExtractionComplete,
        ExtractionTimeSec, false);

    UE_LOG(LogTemp, Log, TEXT("[Match] Extracting at %s..."), *ExtractPointID.ToString());
}

void UBXMatchSubsystem::CancelExtraction()
{
    if (MatchState != EBXMatchState::Extracting) { return; }
    GetGameInstance()->GetWorld()->GetTimerManager().ClearTimer(ExtractTimer);
    SetMatchState(EBXMatchState::InMatch);
    ActiveExtractID = NAME_None;
    UE_LOG(LogTemp, Log, TEXT("[Match] Extraction cancelled"));
}

void UBXMatchSubsystem::OnExtractionComplete()
{
    EndMatch(true, ActiveExtractID);
}

void UBXMatchSubsystem::EndMatch(bool bExtracted, FName ExtractPointID)
{
    if (MatchState == EBXMatchState::PostMatch) { return; }

    UWorld* World = GetGameInstance()->GetWorld();
    World->GetTimerManager().ClearTimer(CountdownTimer);
    World->GetTimerManager().ClearTimer(ExtractTimer);

    CurrentResult.bExtracted    = bExtracted;
    CurrentResult.SurvivalTime  = GetMatchElapsedTime();
    CurrentResult.ExtractPointID = ExtractPointID;
    CurrentResult.ResultMessage = bExtracted
        ? FText::FromString(TEXT("EXTRACTION SUCCESS"))
        : FText::FromString(TEXT("KIA - YOU DIDN'T MAKE IT"));

    LastResult = CurrentResult;
    SetMatchState(EBXMatchState::PostMatch);
    OnMatchEnded.Broadcast(LastResult);

    UE_LOG(LogTemp, Log, TEXT("[Match] ENDED - Extracted=%s KIA=%.1fs Kills=%d Loot=%.0f"),
        bExtracted ? TEXT("YES") : TEXT("NO"),
        CurrentResult.SurvivalTime, CurrentResult.EnemiesKilled, CurrentResult.LootValue);
}

void UBXMatchSubsystem::ReturnToLobby()
{
    SetMatchState(EBXMatchState::Idle);
    UGameplayStatics::OpenLevel(GetGameInstance()->GetWorld(), TEXT("LV_RaidLobby"));
}

float UBXMatchSubsystem::GetMatchElapsedTime() const
{
    if (MatchState == EBXMatchState::Idle || MatchState == EBXMatchState::PreGame)
    {
        return 0.f;
    }
    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (!World) { return 0.f; }
    return World->GetTimeSeconds() - MatchStartTime;
}

void UBXMatchSubsystem::NotifyEnemyKilled()
{
    ++CurrentResult.EnemiesKilled;
}

void UBXMatchSubsystem::NotifyItemLooted(float Value)
{
    CurrentResult.LootValue += Value;
}
