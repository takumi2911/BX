// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXEnemyArchetypeRow.generated.h"

// DT_BX_EnemyArchetypes 行構造体 - 敵キャラクター設定マスタ (Sprint 24)
USTRUCT(BlueprintType)
struct BX_API FBXEnemyArchetypeRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy")
    FName ArchetypeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy")
    FText DisplayName;

    // 初期装備 (NAME_None = 装備なし)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Loadout")
    FName InitialArmorItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Loadout")
    FName InitialWeaponId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Loadout")
    FName InitialAmmoId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Loadout")
    int32 InitialAmmoCount = 30;

    // 部位 HP 倍率 (1.0 = デフォルト値そのまま)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Health")
    float HPMultiplier = 1.0f;

    // 認識 / 行動パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float SightRadius = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float HearingRadius = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float AttackRange = 700.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float AttackCooldownSec = 0.8f;

    // 移動速度 (cm/s): 歩き / 警戒 / 戦闘
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float WalkSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float AlertSpeed = 320.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float CombatSpeed = 420.0f;

    // 命中精度 (0.0 = 完全外す / 1.0 = 必中)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|AI")
    float BaseAccuracy = 0.55f;

    // 報酬
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy|Reward")
    int32 CreditsOnKill = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Enemy")
    FText Description;
};
