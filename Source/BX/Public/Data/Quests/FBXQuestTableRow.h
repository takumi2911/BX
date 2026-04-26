// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Data/BXEnums.h"

#include "FBXQuestTableRow.generated.h"

// 依頼ランタイム進捗 (SPEC §21-2-2) — DataTable 行ではなく Save / Subsystem 内部用構造体
USTRUCT(BlueprintType)
struct BX_API FBXQuestRuntimeProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    int32 CurrentCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    int32 TargetCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    bool bIsAccepted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    bool bIsCompleted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    bool bIsFailed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    int32 AcceptedAtRaidCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    FName LastUpdatedAtRegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    FGameplayTagContainer ProgressTags;

    // 距離 / 重量など補助値
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    float TrackedValueFloat = 0.0f;
};

// DT_BX_Quests 行構造体 — 依頼マスタ (SPEC §21-2-1)
USTRUCT(BlueprintType)
struct BX_API FBXQuestTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FName MerchantId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    EBXQuestType QuestType = EBXQuestType::Collect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FText Description;

    // --- ターゲット ---

    // Collect / Deliver 対象アイテム
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    FName TargetItemId;

    // Kill 対象敵
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    FName TargetEnemyId;

    // Survive / Discover 対象地域
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    FName TargetRegionId;

    // UseExtract 対象抽出 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    FName TargetExtractId;

    // Discover / Survive 対象ストーリーフラグ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    FName TargetStoryFlag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Target")
    int32 TargetCount = 1;

    // --- 開始条件 ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Unlock")
    int32 StartRequiredTrustLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Unlock")
    FName StartRequiredQuestId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Unlock")
    FName StartRequiredStoryFlag;

    // --- 報酬 ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    int32 RewardCredits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    int32 RewardTrustPoints = 0;

    // 完了時に解放する報酬行 ID (DT_BX_MerchantTrustRewards)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    FName RewardUnlockId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    FName RewardItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    int32 RewardItemCount = 0;

    // 完了時に付与するストーリーフラグ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest|Reward")
    FName RewardStoryFlag;

    // --- 失敗 / UI 制御 ---

    // 失敗条件タグ (例: Quest.Fail.ItemBroken)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FGameplayTag FailureRuleTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    bool bIsRepeatable = false;

    // UI 上で主要依頼として強調表示するか
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    bool bIsMainQuestLike = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    int32 PriorityOrder = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Quest")
    FText UIHintText;
};
