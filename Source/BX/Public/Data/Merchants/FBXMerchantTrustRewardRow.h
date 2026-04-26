// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXMerchantTrustRewardRow.generated.h"

// DT_BX_MerchantTrustRewards 行構造体 — Trust 解放内容 (SPEC §21-1-2)
USTRUCT(BlueprintType)
struct BX_API FBXMerchantTrustRewardRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FName RewardId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FName MerchantId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    int32 RequiredTrustLevel = 1;

    // NAME_None なら依頼条件なし
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FName RequiredQuestId;

    // NAME_None なら物語フラグ条件なし
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FName RequiredStoryFlag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    EBXMerchantRewardType RewardType = EBXMerchantRewardType::ItemPool;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FName UnlockTargetId;

    // 整数値報酬 (例: インベントリ枠数)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    int32 RewardValueInt = 0;

    // 小数値報酬 (例: 割引率 0.12 = 12%)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    float RewardValueFloat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    FText UIMessage;

    // true = 未解放でも UI 予告として表示する
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    bool bVisibleBeforeUnlock = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Reward")
    int32 PriorityOrder = 0;
};
