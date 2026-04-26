// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "World/Extract/AC_BX_ExtractEvaluator.h"
#include "Data/Extract/FBXExtractRuleRow.h"
#include "Systems/Merchant/BXMerchantNetworkSubsystem.h"
#include "Systems/Quest/BXQuestSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

static const FName TAG_FAIL_KEY      = TEXT("Extract.Fail.Key");
static const FName TAG_FAIL_MERCHANT = TEXT("Extract.Fail.Merchant");
static const FName TAG_FAIL_QUEST    = TEXT("Extract.Fail.Quest");
static const FName TAG_FAIL_STORY    = TEXT("Extract.Fail.StoryFlag");
static const FName TAG_FAIL_POWER    = TEXT("Extract.Fail.Power");
static const FName TAG_FAIL_ALERT    = TEXT("Extract.Fail.AlertLevel");
static const FName TAG_FAIL_WEIGHT   = TEXT("Extract.Fail.Weight");
static const FName TAG_FAIL_TIME     = TEXT("Extract.Fail.TimeWindow");

FBXExtractEvalResult UAC_BX_ExtractEvaluator::EvaluateConditions(
    FName ExtractId,
    const TArray<FName>& CarriedItemIds,
    int32 AlertLevel,
    float PlayerWeightKg,
    const TArray<FName>& ActivePowerTags,
    FName CurrentTimeTag) const
{
    FBXExtractEvalResult Result;
    const FBXExtractRuleRow* Row = GetRow(ExtractId);
    if (!Row)
    {
        Result.bCanUseNow = false;
        return Result;
    }

    UWorld* World = GetWorld();
    UGameInstance* GI = World ? World->GetGameInstance() : nullptr;

    // 鍵チェック
    if (Row->RequiredKeyItemId != NAME_None)
    {
        Result.bMeetsKeyRequirement = CarriedItemIds.Contains(Row->RequiredKeyItemId);
        if (!Result.bMeetsKeyRequirement)
        {
            Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_KEY));
        }
    }

    // 消耗品チェック (鍵と同じ判定)
    if (Row->RequiredConsumableItemId != NAME_None && !CarriedItemIds.Contains(Row->RequiredConsumableItemId))
    {
        Result.bMeetsKeyRequirement = false;
        Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_KEY));
    }

    // 商人 Trust チェック
    if (Row->RequiredMerchantId != NAME_None && Row->RequiredTrustLevel > 0)
    {
        UBXMerchantNetworkSubsystem* MerchantSub = GI ? GI->GetSubsystem<UBXMerchantNetworkSubsystem>() : nullptr;
        if (!MerchantSub || MerchantSub->GetTrustLevel(Row->RequiredMerchantId) < Row->RequiredTrustLevel)
        {
            Result.bMeetsMerchantRequirement = false;
            Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_MERCHANT));
        }
    }

    // 依頼チェック
    if (Row->RequiredQuestId != NAME_None)
    {
        UBXQuestSubsystem* QuestSub = GI ? GI->GetSubsystem<UBXQuestSubsystem>() : nullptr;
        if (!QuestSub || !QuestSub->IsQuestCompleted(Row->RequiredQuestId))
        {
            Result.bMeetsQuestRequirement = false;
            Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_QUEST));
        }
    }

    // ストーリーフラグチェック
    if (Row->RequiredStoryFlag != NAME_None)
    {
        UBXMerchantNetworkSubsystem* MerchantSub = GI ? GI->GetSubsystem<UBXMerchantNetworkSubsystem>() : nullptr;
        if (!MerchantSub || !MerchantSub->IsContactUnlocked(Row->RequiredMerchantId))
        {
            Result.bMeetsStoryRequirement = false;
            Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_STORY));
        }
    }

    // 通電 / 制御タグチェック
    if (Row->RequiredPowerTag != NAME_None)
    {
        Result.bMeetsPowerRequirement = ActivePowerTags.Contains(Row->RequiredPowerTag);
        if (!Result.bMeetsPowerRequirement)
        {
            Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_POWER));
        }
    }

    // 警報レベルチェック
    if (Row->MaxAllowedAlertLevel > 0 && AlertLevel > Row->MaxAllowedAlertLevel)
    {
        Result.bMeetsAlertRequirement = false;
        Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_ALERT));
    }

    // 重量チェック
    if (Row->RequiredMaxWeightKg > 0.0f && PlayerWeightKg > Row->RequiredMaxWeightKg)
    {
        Result.bMeetsWeightRequirement = false;
        Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_WEIGHT));
    }

    // 時刻帯チェック
    if (Row->RequiredTimeWindowTag != NAME_None && CurrentTimeTag != Row->RequiredTimeWindowTag)
    {
        Result.bMeetsTimeRequirement = false;
        Result.FailureReasonTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_FAIL_TIME));
    }

    Result.bCanUseNow = Result.bMeetsKeyRequirement
        && Result.bMeetsMerchantRequirement
        && Result.bMeetsQuestRequirement
        && Result.bMeetsStoryRequirement
        && Result.bMeetsPowerRequirement
        && Result.bMeetsAlertRequirement
        && Result.bMeetsWeightRequirement
        && Result.bMeetsTimeRequirement;

    return Result;
}

bool UAC_BX_ExtractEvaluator::FindExtractRow(FName ExtractId, FBXExtractEvalResult& OutDefaultResult) const
{
    OutDefaultResult = FBXExtractEvalResult{};
    return GetRow(ExtractId) != nullptr;
}

const FBXExtractRuleRow* UAC_BX_ExtractEvaluator::GetRow(FName ExtractId) const
{
    UDataTable* Table = ExtractRuleDataTable.Get();
    if (!Table) { return nullptr; }
    return Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("AC_BX_ExtractEvaluator"));
}
