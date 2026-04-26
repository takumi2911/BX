// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXExtractRuleRow.generated.h"

// DT_BX_ExtractRules 行構造体 — 抽出条件マスタ (SPEC §18-2)
USTRUCT(BlueprintType)
struct BX_API FBXExtractRuleRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FName ExtractId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FName SubLevelId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    EBXExtractType ExtractType = EBXExtractType::Checkpoint;

    // ワールド上の抽出トリガー Actor タグ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FName TriggerActorTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    bool bVisibleOnMapByDefault = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    EBXExtractMarkerVisibility MarkerVisibilityType = EBXExtractMarkerVisibility::AlwaysVisible;

    // --- 使用条件 ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredKeyItemId;

    // 消費される消耗品 (ロープフックなど)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredConsumableItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredMerchantId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    int32 RequiredTrustLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredQuestId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredStoryFlag;

    // 通電 / 制御タグ (NAME_None = 条件なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredPowerTag;

    // この値以下の警報レベルで使用可能 (0 = 条件なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    int32 MaxAllowedAlertLevel = 0;

    // この重量以下で使用可能 (0 以下 = 制限なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    float RequiredMaxWeightKg = 0.0f;

    // 必要時刻帯タグ (NAME_None = 任意時刻)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Condition")
    FName RequiredTimeWindowTag;

    // --- 待機設定 ---

    // 0 なら即時抽出
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Timer")
    float CountdownTimeSec = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Cancel")
    bool bCancelOnDamage = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Cancel")
    bool bCancelOnMoveOut = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Cancel")
    bool bCancelOnAim = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Cancel")
    bool bCancelOnUseItem = false;

    // true の場合、抽出成功時に RequiredConsumableItemId を消費する
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract|Cancel")
    bool bConsumesRequiredItem = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FName ReinforcementWaveId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    FText UIHintText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    int32 PriorityOrder = 0;

    // true の場合、同レイド内で一度成功した後は使用不可
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    bool bOneWayOnly = false;

    // true の場合 EXリアルモードでは使用不可
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Extract")
    bool bDisabledInExReal = false;
};
