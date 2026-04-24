// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXStatusEffectTableRow.generated.h"

// 状態異常 DataTable 行構造体 (SPEC §15-2)
USTRUCT(BlueprintType)
struct BX_API FBXStatusEffectTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    FName StatusId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    EBXStatusType StatusType = EBXStatusType::Pain;

    // ダメージ/効果をどの間隔(秒)で適用するか
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float TickIntervalSec = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    bool bUsesDuration = true;

    // bUsesDuration=true のとき有効な持続秒数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float BaseDurationSec = 30.0f;

    // 1 Tick あたりの HP ダメージ (0 = なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float DamagePerTick = 0.0f;

    // 移動速度倍率 (1.0 = 変化なし, 0.7 = 30%低下)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float MoveSpeedRatio = 1.0f;

    // ADS 速度倍率
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float ADSSpeedRatio = 1.0f;

    // スタミナ回復倍率
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float StaminaRecoveryRatio = 1.0f;

    // 照準安定倍率 (1.0 = 変化なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float AimStabilityRatio = 1.0f;

    // 音声こもり倍率 (0.0 = 通常, 1.0 = 完全ミュート)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    float AudioMuffleRatio = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    bool bCanStack = false;

    // bCanStack=true のとき有効な最大スタック数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    int32 MaxStackCount = 1;

    // UI 重症度表示 (0=軽度, 1=中度, 2=重度, 3=致命)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status|UI")
    int32 UISeverityLevel = 0;

    // 治療アクション識別子 (BP 側ロジック参照用)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    FName CureActionId;

    // 治療に必要なアイテム ID (FBXItemTableRow の RowName 対応)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    FName RequiredMedItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Status")
    FText Description;
};
