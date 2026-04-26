// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

class UAnimMontage;
class USoundBase;

#include "FBXMedicalItemRow.generated.h"

// DT_BX_MedicalItems 行構造体 — 医療アイテム詳細 (SPEC §15-3)
// DT_BX_Items の ItemId と 1:1 対応する別テーブル
USTRUCT(BlueprintType)
struct BX_API FBXMedicalItemRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    FName MedicalItemId;

    // DT_BX_Items の対応 ItemId
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    EBXMedicalType MedicalType = EBXMedicalType::Bandage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    FText DisplayName;

    // 使用開始から完了までの秒数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Timing")
    float UseTimeSec = 2.0f;

    // 完了後の再使用クールダウン（秒）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Timing")
    float CooldownSec = 0.0f;

    // 1 個あたりの最大使用回数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    int32 MaxCharges = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    float RestoresHP = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    float RestoresHydration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    float RestoresEnergy = 0.0f;

    // 疼痛軽減係数 (0.0 = 効果なし, 1.0 = 完全除去)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    float PainReliefRatio = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    bool bStopsSmallBleed = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    bool bStopsLargeBleed = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    bool bFixesFracture = false;

    // true の場合のみ Neuro Critical を解除できる
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    bool bCanResolveNeuroCritical = false;

    // 解除対象の状態異常 ID 配列 (CSV: "(id1,id2)" 形式)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    TArray<FName> CuresStatusIds;

    // 使用後に付与するバフ / デバフの状態異常 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Effect")
    FName AppliedBuffStatusId;

    // --- 中断条件 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Cancel")
    bool bCancelOnDamage = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Cancel")
    bool bCancelOnSprint = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Cancel")
    bool bCancelOnWeaponUse = true;

    // 0 = 無制限
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical|Rules")
    int32 MaxCarryPerRaid = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    FName UseActionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UAnimMontage> UseMontage1P;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USoundBase> UseSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Medical")
    FText Description;
};
