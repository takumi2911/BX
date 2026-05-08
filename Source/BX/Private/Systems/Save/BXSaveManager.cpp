// Copyright (c) takumi2911 - BlackoutExfil
#include "Systems/Save/BXSaveManager.h"
#include "Save/USaveGame_BX.h"
#include "Kismet/GameplayStatics.h"

void UBXSaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

FString UBXSaveManager::GetSlotName(const FString& SlotSuffix) const
{
    return USaveGame_BX::SlotNamePrefix + SlotSuffix;
}

void UBXSaveManager::SaveGame(const FString& SlotSuffix)
{
    if (!CurrentSave)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BXSaveManager] SaveGame: CurrentSave is null"));
        OnSaveComplete.Broadcast(false);
        return;
    }

    CurrentSave->ProfileMeta.LastPlayedAt = FDateTime::UtcNow();
    const bool bOk = UGameplayStatics::SaveGameToSlot(CurrentSave, GetSlotName(SlotSuffix), 0);
    UE_LOG(LogTemp, Log, TEXT("[BXSaveManager] SaveGame -> slot=%s ok=%d"), *SlotSuffix, bOk);
    OnSaveComplete.Broadcast(bOk);
}

void UBXSaveManager::LoadGame(const FString& SlotSuffix)
{
    USaveGame* Raw = UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotSuffix), 0);
    CurrentSave    = Cast<USaveGame_BX>(Raw);

    const bool bOk = (CurrentSave != nullptr);
    UE_LOG(LogTemp, Log, TEXT("[BXSaveManager] LoadGame -> slot=%s ok=%d"), *SlotSuffix, bOk);
    OnLoadComplete.Broadcast(bOk, CurrentSave);
}

void UBXSaveManager::NewGame(const FString& ProfileName, EBXGameMode GameMode)
{
    CurrentSave = Cast<USaveGame_BX>(
        UGameplayStatics::CreateSaveGameObject(USaveGame_BX::StaticClass()));

    CurrentSave->ProfileMeta.ProfileId    = FGuid::NewGuid();
    CurrentSave->ProfileMeta.ProfileName  = ProfileName;
    CurrentSave->ProfileMeta.GameMode     = GameMode;
    CurrentSave->ProfileMeta.CreatedAt    = FDateTime::UtcNow();
    CurrentSave->ProfileMeta.LastPlayedAt = FDateTime::UtcNow();
    CurrentSave->ProfileMeta.SaveVersion  = 1;

    UE_LOG(LogTemp, Log, TEXT("[BXSaveManager] NewGame -> profile=%s"), *ProfileName);
}

bool UBXSaveManager::DoesSaveExist(const FString& SlotSuffix) const
{
    return UGameplayStatics::DoesSaveGameExist(GetSlotName(SlotSuffix), 0);
}

void UBXSaveManager::AutoSaveOnRaidEnd()
{
    if (!CurrentSave)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BXSaveManager] AutoSaveOnRaidEnd: no active save"));
        return;
    }

    ++CurrentSave->ProfileMeta.TotalRaids;
    CurrentSave->RunSnapshot.bHasActiveRun = false;
    SaveGame();
}

void UBXSaveManager::SaveOnExtractSuccess()
{
    if (!CurrentSave) { return; }

    ++CurrentSave->ProfileMeta.TotalRaids;
    CurrentSave->RunSnapshot = FBXRunSnapshot();
    SaveGame();
}
