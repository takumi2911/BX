// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Systems/Quest/BXQuestSubsystem.h"
#include "Systems/Merchant/BXMerchantNetworkSubsystem.h"
#include "Save/FBXMerchantNetworkState.h"
#include "Engine/DataTable.h"

void UBXQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UBXQuestSubsystem::CanAcceptQuest(FName QuestId) const
{
    const FBXQuestTableRow* Row = FindRow(QuestId);
    if (!Row) { return false; }

    // 既に受注中 / 完了済み (非繰り返し)
    if (ActiveProgress.Contains(QuestId)) { return false; }
    if (!Row->bIsRepeatable && CompletedQuestIds.Contains(QuestId)) { return false; }
    if (FailedQuestIds.Contains(QuestId)) { return false; }

    // Trust レベル確認
    UBXMerchantNetworkSubsystem* MerchantSub = GetMerchantSubsystem();
    if (MerchantSub && MerchantSub->GetTrustLevel(Row->MerchantId) < Row->StartRequiredTrustLevel) { return false; }

    // 前提依頼
    if (Row->StartRequiredQuestId != NAME_None && !CompletedQuestIds.Contains(Row->StartRequiredQuestId)) { return false; }

    // 前提ストーリーフラグ
    if (Row->StartRequiredStoryFlag != NAME_None)
    {
        if (!MerchantSub || !MerchantSub->IsContactUnlocked(Row->MerchantId)) { return false; }
    }

    return true;
}

bool UBXQuestSubsystem::AcceptQuest(FName QuestId)
{
    if (!CanAcceptQuest(QuestId)) { return false; }

    const FBXQuestTableRow* Row = FindRow(QuestId);
    if (!Row) { return false; }

    FBXQuestRuntimeProgress Progress;
    Progress.QuestId     = QuestId;
    Progress.TargetCount = Row->TargetCount;
    Progress.bIsAccepted = true;

    ActiveProgress.Add(QuestId, Progress);
    OnQuestAccepted.Broadcast(QuestId);
    return true;
}

void UBXQuestSubsystem::AddQuestProgress(FName QuestId, int32 Delta)
{
    FBXQuestRuntimeProgress* Progress = ActiveProgress.Find(QuestId);
    if (!Progress || Progress->bIsCompleted || Progress->bIsFailed) { return; }

    Progress->CurrentCount = FMath::Clamp(Progress->CurrentCount + Delta, 0, Progress->TargetCount);
    OnQuestProgress.Broadcast(QuestId, Progress->CurrentCount);

    if (Progress->CurrentCount >= Progress->TargetCount)
    {
        FinishQuest(QuestId);
    }
}

void UBXQuestSubsystem::AddQuestProgressTag(FName QuestId, FGameplayTag Tag)
{
    FBXQuestRuntimeProgress* Progress = ActiveProgress.Find(QuestId);
    if (!Progress || Progress->bIsCompleted || Progress->bIsFailed) { return; }

    if (!Progress->ProgressTags.HasTag(Tag))
    {
        Progress->ProgressTags.AddTag(Tag);
        AddQuestProgress(QuestId, 1);
    }
}

void UBXQuestSubsystem::TriggerFailureByTag(FGameplayTag FailureTag)
{
    TArray<FName> ToFail;
    for (const auto& Pair : ActiveProgress)
    {
        const FBXQuestTableRow* Row = FindRow(Pair.Key);
        if (Row && Row->FailureRuleTag == FailureTag && !Pair.Value.bIsCompleted)
        {
            ToFail.Add(Pair.Key);
        }
    }

    for (FName QuestId : ToFail)
    {
        FBXQuestRuntimeProgress* Progress = ActiveProgress.Find(QuestId);
        if (Progress)
        {
            Progress->bIsFailed = true;
            FailedQuestIds.AddUnique(QuestId);
            ActiveProgress.Remove(QuestId);
            OnQuestFailed.Broadcast(QuestId);
        }
    }
}

bool UBXQuestSubsystem::IsQuestActive(FName QuestId) const
{
    return ActiveProgress.Contains(QuestId);
}

bool UBXQuestSubsystem::IsQuestCompleted(FName QuestId) const
{
    return CompletedQuestIds.Contains(QuestId);
}

bool UBXQuestSubsystem::IsQuestFailed(FName QuestId) const
{
    return FailedQuestIds.Contains(QuestId);
}

FBXQuestRuntimeProgress UBXQuestSubsystem::GetQuestProgress(FName QuestId) const
{
    const FBXQuestRuntimeProgress* Found = ActiveProgress.Find(QuestId);
    return Found ? *Found : FBXQuestRuntimeProgress{};
}

TArray<FName> UBXQuestSubsystem::GetActiveQuestIds() const
{
    TArray<FName> Keys;
    ActiveProgress.GetKeys(Keys);
    return Keys;
}

TArray<FName> UBXQuestSubsystem::GetAvailableQuestIdsForMerchant(FName MerchantId) const
{
    TArray<FName> Result;
    UDataTable* Table = QuestDataTable.Get();
    if (!Table) { return Result; }

    for (FName RowName : Table->GetRowNames())
    {
        const FBXQuestTableRow* Row = Table->FindRow<FBXQuestTableRow>(RowName, TEXT(""));
        if (Row && Row->MerchantId == MerchantId && CanAcceptQuest(RowName))
        {
            Result.Add(RowName);
        }
    }
    Result.Sort([Table](const FName& A, const FName& B)
    {
        const FBXQuestTableRow* RA = Table->FindRow<FBXQuestTableRow>(A, TEXT(""));
        const FBXQuestTableRow* RB = Table->FindRow<FBXQuestTableRow>(B, TEXT(""));
        return RA && RB && RA->PriorityOrder < RB->PriorityOrder;
    });
    return Result;
}

void UBXQuestSubsystem::SaveQuestState(FBXMerchantNetworkState& OutState) const
{
    // 依頼進行状態はマーチャント別 ActiveQuests / CompletedQuests / FailedQuests に同期
    for (auto& Pair : OutState.MerchantStates)
    {
        Pair.Value.ActiveQuests.Reset();
        Pair.Value.CompletedQuests = CompletedQuestIds; // 全依頼を共有
        Pair.Value.FailedQuests    = FailedQuestIds;
    }
    for (const auto& Pair : ActiveProgress)
    {
        const FBXQuestTableRow* Row = FindRow(Pair.Key);
        if (!Row) { continue; }
        if (FBXMerchantState* State = OutState.MerchantStates.Find(Row->MerchantId))
        {
            State->ActiveQuests.AddUnique(Pair.Key);
        }
    }
}

void UBXQuestSubsystem::LoadQuestState(const FBXMerchantNetworkState& InState)
{
    ActiveProgress.Reset();
    CompletedQuestIds.Reset();
    FailedQuestIds.Reset();

    for (const auto& Pair : InState.MerchantStates)
    {
        for (FName QuestId : Pair.Value.ActiveQuests)
        {
            if (!ActiveProgress.Contains(QuestId))
            {
                const FBXQuestTableRow* Row = FindRow(QuestId);
                FBXQuestRuntimeProgress Progress;
                Progress.QuestId     = QuestId;
                Progress.TargetCount = Row ? Row->TargetCount : 1;
                Progress.bIsAccepted = true;
                ActiveProgress.Add(QuestId, Progress);
            }
        }
        for (FName QuestId : Pair.Value.CompletedQuests)
        {
            CompletedQuestIds.AddUnique(QuestId);
        }
        for (FName QuestId : Pair.Value.FailedQuests)
        {
            FailedQuestIds.AddUnique(QuestId);
        }
    }
}

void UBXQuestSubsystem::ResetForNewProfile()
{
    ActiveProgress.Reset();
    CompletedQuestIds.Reset();
    FailedQuestIds.Reset();
}

// --- Private ---

const FBXQuestTableRow* UBXQuestSubsystem::FindRow(FName QuestId) const
{
    UDataTable* Table = QuestDataTable.Get();
    if (!Table) { return nullptr; }
    return Table->FindRow<FBXQuestTableRow>(QuestId, TEXT("BXQuestSubsystem"));
}

UBXMerchantNetworkSubsystem* UBXQuestSubsystem::GetMerchantSubsystem() const
{
    UGameInstance* GI = GetGameInstance();
    return GI ? GI->GetSubsystem<UBXMerchantNetworkSubsystem>() : nullptr;
}

void UBXQuestSubsystem::FinishQuest(FName QuestId)
{
    FBXQuestRuntimeProgress* Progress = ActiveProgress.Find(QuestId);
    if (!Progress) { return; }

    Progress->bIsCompleted = true;
    CompletedQuestIds.AddUnique(QuestId);

    const FBXQuestTableRow* Row = FindRow(QuestId);
    if (Row)
    {
        DistributeRewards(*Row);
    }

    ActiveProgress.Remove(QuestId);
    OnQuestCompleted.Broadcast(QuestId);
}

void UBXQuestSubsystem::DistributeRewards(const FBXQuestTableRow& Row)
{
    UBXMerchantNetworkSubsystem* MerchantSub = GetMerchantSubsystem();
    if (!MerchantSub) { return; }

    if (Row.RewardTrustPoints > 0)
    {
        MerchantSub->AddTrustPoints(Row.MerchantId, Row.RewardTrustPoints);
    }

    MerchantSub->SetQuestCompleted(Row.QuestId, Row.MerchantId);

    if (Row.RewardStoryFlag != NAME_None)
    {
        MerchantSub->SetStoryFlagActive(Row.RewardStoryFlag);
    }
    // RewardCredits / RewardItemId は Inventory / Economy サブシステムが担当 (将来実装)
}
