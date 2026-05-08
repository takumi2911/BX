// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXDroneArchetypeRow.generated.h"

// DT_BX_DroneArchetypes 行構造体 - ドローン設定マスタ (Sprint 26)
USTRUCT(BlueprintType)
struct BX_API FBXDroneArchetypeRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone")
    FName ArchetypeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone")
    FText DisplayName;

    // ドローン種別 (偵察 / 攻撃)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone")
    bool bIsAttackDrone = false;

    // HP
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Combat")
    float MaxHP = 150.0f;

    // 移動 (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Movement")
    float PatrolSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Movement")
    float TrackSpeed  = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Movement")
    float AttackSpeed = 800.0f;

    // 飛行高度 (cm): デフォルト地上からの高さ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Movement")
    float HoverHeight = 250.0f;

    // 感知
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|AI")
    float SightRadius = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|AI")
    float SightAngleDegrees = 90.0f;

    // 攻撃 (攻撃ドローンのみ有効)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Combat")
    float AttackRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Combat")
    float AttackDamage = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Combat")
    float AttackCooldownSec = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone|Combat")
    float BaseAccuracy = 0.6f;

    // 報酬
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone")
    int32 CreditsOnKill = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Drone")
    FText Description;
};
