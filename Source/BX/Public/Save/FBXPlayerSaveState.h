// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

#include "FBXPlayerSaveState.generated.h"

// §16-4 プレイヤー部位 HP 保存状態 (スタブ — Sprint XX で拡張)
USTRUCT(BlueprintType)
struct BX_API FBXHealthState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Health")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Health")
    bool bIsAlive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Health")
    bool bIsUnconscious = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Health")
    int32 OverallPainTier = 0;
};

// §16-4 プレイヤー状態異常保存状態 (スタブ — Sprint XX で拡張)
USTRUCT(BlueprintType)
struct BX_API FBXStatusEffectsState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Status")
    int32 SaveVersion = 1;
};

// §16-4 プレイヤーインベントリ保存状態 (スタブ — Sprint XX で拡張)
USTRUCT(BlueprintType)
struct BX_API FBXInventoryState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    float TotalWeightKg = 0.0f;
};

// §16-4 武器ハンドラー保存状態 (スタブ — Sprint 14 で拡張)
USTRUCT(BlueprintType)
struct BX_API FBXWeaponHandlerState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Weapon")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Weapon")
    EBXWeaponSlot CurrentSlot = EBXWeaponSlot::Primary;
};

// §16-4 プレイヤー全体保存状態 (SPEC §23-4 ExportPlayerSaveState / ImportPlayerSaveState 用)
USTRUCT(BlueprintType)
struct BX_API FBXPlayerSaveState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    FTransform SavedTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    EBXViewMode SavedViewMode = EBXViewMode::First;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    FBXHealthState SavedHealthState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    FBXStatusEffectsState SavedStatusEffectsState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    FBXInventoryState SavedInventoryState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Player")
    FBXWeaponHandlerState SavedWeaponState;
};
