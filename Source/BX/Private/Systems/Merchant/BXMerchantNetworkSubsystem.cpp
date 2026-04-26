// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Systems/Merchant/BXMerchantNetworkSubsystem.h"
#include "Data/Merchants/FBXMerchantTableRow.h"
#include "Data/Merchants/FBXMerchantTrustRewardRow.h"
#include "Engine/DataTable.h"

void UBXMerchantNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UBXMerchantNetworkSubsystem::AddTrustPoints(FName MerchantId, int32 Points)
{
    FBXMerchantState& State = GetOrCreateMerchantState(MerchantId);
    State.TrustPoints += Points;

    int32 NewLevel = ComputeTrustLevel(MerchantId, State.TrustPoints);
    if (NewLevel != State.TrustLevel)
    {
        State.TrustLevel = NewLevel;
        OnTrustLevelChanged.Broadcast(MerchantId, NewLevel);
    }
    CheckAndApplyNewRewards(MerchantId);
}

int32 UBXMerchantNetworkSubsystem::GetTrustLevel(FName MerchantId) const
{
    const FBXMerchantState* State = NetworkState.MerchantStates.Find(MerchantId);
    return State ? State->TrustLevel : 1;
}

int32 UBXMerchantNetworkSubsystem::GetTrustPoints(FName MerchantId) const
{
    const FBXMerchantState* State = NetworkState.MerchantStates.Find(MerchantId);
    return State ? State->TrustPoints : 0;
}

float UBXMerchantNetworkSubsystem::GetTrustProgressRatio(FName MerchantId) const
{
    const FBXMerchantTableRow* Row = FindMerchantRow(MerchantId);
    if (!Row) { return 0.0f; }

    int32 Points = GetTrustPoints(MerchantId);
    int32 Level  = GetTrustLevel(MerchantId);

    int32 LowThreshold  = 0;
    int32 HighThreshold = 0;

    switch (Level)
    {
    case 1: LowThreshold = 0;                    HighThreshold = Row->Trust2Threshold; break;
    case 2: LowThreshold = Row->Trust2Threshold; HighThreshold = Row->Trust3Threshold; break;
    case 3: LowThreshold = Row->Trust3Threshold; HighThreshold = Row->Trust4Threshold; break;
    case 4: return 1.0f;
    default: return 0.0f;
    }

    int32 Range = HighThreshold - LowThreshold;
    if (Range <= 0) { return 0.0f; }
    return FMath::Clamp(static_cast<float>(Points - LowThreshold) / static_cast<float>(Range), 0.0f, 1.0f);
}

bool UBXMerchantNetworkSubsystem::IsRewardUnlocked(FName RewardId) const
{
    return AppliedRewardIds.Contains(RewardId);
}

TArray<FName> UBXMerchantNetworkSubsystem::GetUnlockedRewardIds(FName MerchantId) const
{
    TArray<FName> Result;
    UDataTable* Table = TrustRewardDataTable.Get();
    if (!Table) { return Result; }

    for (FName RowName : Table->GetRowNames())
    {
        const FBXMerchantTrustRewardRow* Row = Table->FindRow<FBXMerchantTrustRewardRow>(RowName, TEXT(""));
        if (Row && Row->MerchantId == MerchantId && AppliedRewardIds.Contains(RowName))
        {
            Result.Add(RowName);
        }
    }

    Result.Sort([Table](const FName& A, const FName& B)
    {
        const FBXMerchantTrustRewardRow* RA = Table->FindRow<FBXMerchantTrustRewardRow>(A, TEXT(""));
        const FBXMerchantTrustRewardRow* RB = Table->FindRow<FBXMerchantTrustRewardRow>(B, TEXT(""));
        return RA && RB && RA->PriorityOrder < RB->PriorityOrder;
    });
    return Result;
}

TArray<FName> UBXMerchantNetworkSubsystem::GetVisiblePendingRewardIds(FName MerchantId) const
{
    TArray<FName> Result;
    UDataTable* Table = TrustRewardDataTable.Get();
    if (!Table) { return Result; }

    for (FName RowName : Table->GetRowNames())
    {
        const FBXMerchantTrustRewardRow* Row = Table->FindRow<FBXMerchantTrustRewardRow>(RowName, TEXT(""));
        if (Row && Row->MerchantId == MerchantId && !AppliedRewardIds.Contains(RowName) && Row->bVisibleBeforeUnlock)
        {
            Result.Add(RowName);
        }
    }

    Result.Sort([Table](const FName& A, const FName& B)
    {
        const FBXMerchantTrustRewardRow* RA = Table->FindRow<FBXMerchantTrustRewardRow>(A, TEXT(""));
        const FBXMerchantTrustRewardRow* RB = Table->FindRow<FBXMerchantTrustRewardRow>(B, TEXT(""));
        return RA && RB && RA->PriorityOrder < RB->PriorityOrder;
    });
    return Result;
}

bool UBXMerchantNetworkSubsystem::IsContactUnlocked(FName MerchantId) const
{
    const FBXMerchantTableRow* Row = FindMerchantRow(MerchantId);
    if (!Row) { return false; }
    if (Row->StoryFlagRequiredForContact == NAME_None) { return true; }
    return ActiveStoryFlags.Contains(Row->StoryFlagRequiredForContact);
}

void UBXMerchantNetworkSubsystem::SetQuestCompleted(FName QuestId, FName MerchantId)
{
    FBXMerchantState& State = GetOrCreateMerchantState(MerchantId);
    State.ActiveQuests.Remove(QuestId);
    State.CompletedQuests.AddUnique(QuestId);
    CheckAndApplyNewRewards(MerchantId);
}

void UBXMerchantNetworkSubsystem::SetStoryFlagActive(FName StoryFlag)
{
    if (ActiveStoryFlags.Contains(StoryFlag)) { return; }
    ActiveStoryFlags.Add(StoryFlag);

    UDataTable* MTable = MerchantDataTable.Get();
    if (!MTable) { return; }

    for (FName RowName : MTable->GetRowNames())
    {
        CheckAndApplyNewRewards(RowName);
    }
}

void UBXMerchantNetworkSubsystem::SaveToNetworkState(FBXMerchantNetworkState& OutState) const
{
    OutState = NetworkState;
}

void UBXMerchantNetworkSubsystem::LoadFromNetworkState(const FBXMerchantNetworkState& InState)
{
    NetworkState = InState;
    AppliedRewardIds.Reset();

    UDataTable* Table = TrustRewardDataTable.Get();
    if (!Table) { return; }

    // 条件を満たしている報酬を AppliedRewardIds に復元 (効果は既に NetworkState 内)
    for (FName RowName : Table->GetRowNames())
    {
        const FBXMerchantTrustRewardRow* Row = Table->FindRow<FBXMerchantTrustRewardRow>(RowName, TEXT(""));
        if (Row && AreRewardConditionsMet(Row->MerchantId, *Row))
        {
            AppliedRewardIds.Add(RowName);
        }
    }
}

void UBXMerchantNetworkSubsystem::ResetForNewProfile()
{
    NetworkState    = FBXMerchantNetworkState{};
    ActiveStoryFlags.Reset();
    AppliedRewardIds.Reset();
}

// --- Private ---

FBXMerchantState& UBXMerchantNetworkSubsystem::GetOrCreateMerchantState(FName MerchantId)
{
    if (FBXMerchantState* Existing = NetworkState.MerchantStates.Find(MerchantId))
    {
        return *Existing;
    }

    FBXMerchantState NewState;
    if (const FBXMerchantTableRow* Row = FindMerchantRow(MerchantId))
    {
        NewState.TrustLevel  = Row->InitialTrustLevel;
        NewState.TrustPoints = Row->InitialTrustPoints;
    }
    return NetworkState.MerchantStates.Add(MerchantId, MoveTemp(NewState));
}

int32 UBXMerchantNetworkSubsystem::ComputeTrustLevel(FName MerchantId, int32 TrustPoints) const
{
    const FBXMerchantTableRow* Row = FindMerchantRow(MerchantId);
    if (!Row) { return 1; }
    if (TrustPoints >= Row->Trust4Threshold) { return 4; }
    if (TrustPoints >= Row->Trust3Threshold) { return 3; }
    if (TrustPoints >= Row->Trust2Threshold) { return 2; }
    return 1;
}

void UBXMerchantNetworkSubsystem::CheckAndApplyNewRewards(FName MerchantId)
{
    UDataTable* Table = TrustRewardDataTable.Get();
    if (!Table) { return; }

    for (FName RowName : Table->GetRowNames())
    {
        const FBXMerchantTrustRewardRow* Row = Table->FindRow<FBXMerchantTrustRewardRow>(RowName, TEXT(""));
        if (!Row || Row->MerchantId != MerchantId || AppliedRewardIds.Contains(RowName)) { continue; }

        if (AreRewardConditionsMet(MerchantId, *Row))
        {
            ApplyReward(MerchantId, *Row);
            AppliedRewardIds.Add(RowName);
            OnRewardUnlocked.Broadcast(MerchantId, RowName);
        }
    }
}

void UBXMerchantNetworkSubsystem::ApplyReward(FName /*MerchantId*/, const FBXMerchantTrustRewardRow& Row)
{
    if (Row.RewardType == EBXMerchantRewardType::RespawnNode)
    {
        NetworkState.UnlockedRespawnNodes.AddUnique(Row.UnlockTargetId);
    }
    // 他の RewardType (Service / Discount / ItemPool 等) は条件充足から導出するため追記不要
}

bool UBXMerchantNetworkSubsystem::AreRewardConditionsMet(FName MerchantId, const FBXMerchantTrustRewardRow& Row) const
{
    if (GetTrustLevel(MerchantId) < Row.RequiredTrustLevel) { return false; }

    if (Row.RequiredQuestId != NAME_None)
    {
        const FBXMerchantState* State = NetworkState.MerchantStates.Find(MerchantId);
        if (!State || !State->CompletedQuests.Contains(Row.RequiredQuestId)) { return false; }
    }

    if (Row.RequiredStoryFlag != NAME_None && !ActiveStoryFlags.Contains(Row.RequiredStoryFlag))
    {
        return false;
    }

    return true;
}

const FBXMerchantTableRow* UBXMerchantNetworkSubsystem::FindMerchantRow(FName MerchantId) const
{
    UDataTable* Table = MerchantDataTable.Get();
    if (!Table) { return nullptr; }
    return Table->FindRow<FBXMerchantTableRow>(MerchantId, TEXT("BXMerchantNetworkSubsystem"));
}
