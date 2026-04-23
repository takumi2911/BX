// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

#include "FBXInventoryTypes.generated.h"

// インベントリスロット定義 (SPEC §15-1-1)
USTRUCT(BlueprintType)
struct BX_API FBXInventorySlotDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    FName SlotId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    EBXInventorySlotType SlotType = EBXInventorySlotType::Backpack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    int32 CapacityUnits = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    bool bAcceptsWeapon = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    bool bAcceptsMagazine = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    bool bAcceptsConsumable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    bool bAcceptsQuestItem = false;
};

// ランタイム / セーブ共用アイテムスタック (SPEC §15-1-2)
// Sprint 3 の FBXSavedItemStack を統合。セーブとランタイムで単一型を使用。
USTRUCT(BlueprintType)
struct BX_API FBXInventoryItemStack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    float CurrentDurability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    int32 AmmoLoadedCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    FName CustomTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    FName SourceRegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory")
    bool bIsFoundInRaid = false;
};
