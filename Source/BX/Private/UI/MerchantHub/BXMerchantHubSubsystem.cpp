// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "UI/MerchantHub/BXMerchantHubSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/Merchants/FBXMerchantTableRow.h"
#include "Save/FBXMerchantNetworkState.h"
#include "Systems/Merchant/BXMerchantNetworkSubsystem.h"
#include "Systems/Quest/BXQuestSubsystem.h"

void UBXMerchantHubSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UBXMerchantHubSubsystem::SelectMerchant(FName MerchantId)
{
    SelectedMerchantId = MerchantId;
}

FBXMerchantHubHomeViewModel UBXMerchantHubSubsystem::BuildHomeViewModel() const
{
    FBXMerchantHubHomeViewModel VM;
    VM.MerchantId      = SelectedMerchantId;
    VM.PlayerCredits   = PlayerCredits;
    VM.CurrentRegionId = CurrentRegionId;
    VM.RecentLogLines  = RecentLogLines;
    VM.WarningTags     = WarningTags;

    UBXMerchantNetworkSubsystem* NetSub = GetMerchantNetworkSub();
    UBXQuestSubsystem*           QuestSub = GetQuestSub();

    if (NetSub)
    {
        VM.TrustLevel   = NetSub->GetTrustLevel(SelectedMerchantId);
        VM.TrustPoints  = NetSub->GetTrustPoints(SelectedMerchantId);

        // Derive TrustPointsToNext from the DataTable thresholds
        UDataTable* DT = MerchantDataTable.Get();
        if (DT)
        {
            const FBXMerchantTableRow* Row = DT->FindRow<FBXMerchantTableRow>(SelectedMerchantId, TEXT("BuildHomeViewModel"));
            if (Row)
            {
                VM.MerchantDisplayName = Row->DisplayName;
                VM.MerchantTagline     = Row->Tagline;

                const int32 CurrentPoints = VM.TrustPoints;
                const int32 Level         = VM.TrustLevel;

                if (Level == 1)      { VM.TrustPointsToNext = FMath::Max(0, Row->Trust2Threshold - CurrentPoints); }
                else if (Level == 2) { VM.TrustPointsToNext = FMath::Max(0, Row->Trust3Threshold - CurrentPoints); }
                else if (Level == 3) { VM.TrustPointsToNext = FMath::Max(0, Row->Trust4Threshold - CurrentPoints); }
                else                 { VM.TrustPointsToNext = 0; }
            }
        }

        // Count unlocked respawn nodes for this merchant
        TArray<FName> UnlockedRewards = NetSub->GetUnlockedRewardIds(SelectedMerchantId);
        VM.UnlockedRespawnNodeCount   = UnlockedRewards.Num();

        // Count stored inventory items
        FBXMerchantNetworkState TempState;
        NetSub->SaveToNetworkState(TempState);
        if (const FBXMerchantState* MState = TempState.MerchantStates.Find(SelectedMerchantId))
        {
            VM.StoredInventoryCount = MState->StoredInventory.Num();
        }
    }

    if (QuestSub)
    {
        TArray<FName> ActiveIds = QuestSub->GetActiveQuestIds();
        int32 Count = 0;
        for (const FName& QuestId : ActiveIds)
        {
            FBXQuestRuntimeProgress Progress = QuestSub->GetQuestProgress(QuestId);
            if (!Progress.bIsCompleted && !Progress.bIsFailed)
            {
                ++Count;
            }
        }
        VM.ActiveQuestCount = Count;
    }

    return VM;
}

TArray<FBXMerchantHubMerchantRowViewModel> UBXMerchantHubSubsystem::BuildMerchantList() const
{
    TArray<FBXMerchantHubMerchantRowViewModel> Result;

    UDataTable* DT = MerchantDataTable.Get();
    if (!DT)
    {
        return Result;
    }

    UBXMerchantNetworkSubsystem* NetSub   = GetMerchantNetworkSub();
    UBXQuestSubsystem*           QuestSub = GetQuestSub();

    TArray<FName> RowNames = DT->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FBXMerchantTableRow* Row = DT->FindRow<FBXMerchantTableRow>(RowName, TEXT("BuildMerchantList"));
        if (!Row)
        {
            continue;
        }

        FBXMerchantHubMerchantRowViewModel RowVM;
        RowVM.MerchantId   = Row->MerchantId;
        RowVM.DisplayName  = Row->DisplayName;
        RowVM.IconTexture  = Row->PortraitTexture;
        RowVM.TrustLevel   = NetSub ? NetSub->GetTrustLevel(Row->MerchantId) : Row->InitialTrustLevel;

        if (QuestSub)
        {
            TArray<FName> Available = QuestSub->GetAvailableQuestIdsForMerchant(Row->MerchantId);
            RowVM.bHasNewQuest = Available.Num() > 0;
        }

        RowVM.bHasUrgentWarning = false;

        Result.Add(RowVM);
    }

    // Sort by TrustLevel ascending (SPEC §29-3: Trust 昇順)
    Result.Sort([](const FBXMerchantHubMerchantRowViewModel& A, const FBXMerchantHubMerchantRowViewModel& B)
    {
        return A.TrustLevel < B.TrustLevel;
    });

    return Result;
}

void UBXMerchantHubSubsystem::SetPlayerCredits(int32 Credits)
{
    PlayerCredits = Credits;
}

void UBXMerchantHubSubsystem::SetCurrentRegion(FName RegionId)
{
    CurrentRegionId = RegionId;
}

void UBXMerchantHubSubsystem::AddWarningTag(FGameplayTag Tag)
{
    WarningTags.AddTag(Tag);
}

void UBXMerchantHubSubsystem::ClearWarningTags()
{
    WarningTags.Reset();
}

void UBXMerchantHubSubsystem::AddRecentLogLine(FText Line)
{
    RecentLogLines.Insert(Line, 0);
    if (RecentLogLines.Num() > MaxLogLines)
    {
        RecentLogLines.SetNum(MaxLogLines);
    }
}

void UBXMerchantHubSubsystem::ClearRecentLog()
{
    RecentLogLines.Empty();
}

UBXMerchantNetworkSubsystem* UBXMerchantHubSubsystem::GetMerchantNetworkSub() const
{
    UGameInstance* GI = GetGameInstance();
    return GI ? GI->GetSubsystem<UBXMerchantNetworkSubsystem>() : nullptr;
}

UBXQuestSubsystem* UBXMerchantHubSubsystem::GetQuestSub() const
{
    UGameInstance* GI = GetGameInstance();
    return GI ? GI->GetSubsystem<UBXQuestSubsystem>() : nullptr;
}
