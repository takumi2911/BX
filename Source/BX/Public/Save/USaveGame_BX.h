// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save/FBXProfileMeta.h"
#include "Save/FBXGlobalSettings.h"
#include "Save/FBXMerchantNetworkState.h"
#include "Save/FBXPlayerProgress.h"
#include "Save/FBXRegionState.h"
#include "Save/FBXRunSnapshot.h"

#include "USaveGame_BX.generated.h"

// BX メインセーブゲームクラス（全構造体を統合）(CLAUDE.md §6 / SPEC_GAPS G-01)
// 通常はバイナリセーブ。デバッグ時のみ DebugDumpJson() で JSON ダンプ可能。
UCLASS(BlueprintType)
class BX_API USaveGame_BX : public USaveGame
{
    GENERATED_BODY()

public:
    USaveGame_BX();

    // --- 保存スロット識別 ---
    static const FString SlotNamePrefix;

    // --- ブロック 1: プロファイル識別・統計 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    FBXProfileMeta ProfileMeta;

    // --- ブロック 2: グローバル設定 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    FBXGlobalSettings GlobalSettings;

    // --- ブロック 3: 商人ネットワーク状態 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    FBXMerchantNetworkState MerchantNetwork;

    // --- ブロック 4: プレイヤー進行 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    FBXPlayerProgress PlayerProgress;

    // --- ブロック 5: 地域状態（地域ごとにマップ管理）---
    // RegionId → FBXRegionState
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    TMap<FName, FBXRegionState> RegionStates;

    // --- ブロック 6: レイド中断スナップショット ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save")
    FBXRunSnapshot RunSnapshot;

    // デバッグ用 JSON ダンプ（FJsonObjectConverter 利用）
    UFUNCTION(BlueprintCallable, Category="BX|Save|Debug")
    FString DebugDumpJson() const;
};
