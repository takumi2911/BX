// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Data/BXEnums.h"

class UTexture2D;
class UStaticMesh;

#include "FBXItemTableRow.generated.h"

// DT_BX_Items の行構造体 — 全アイテム共通マスタ (SPEC §15-1)
USTRUCT(BlueprintType)
struct BX_API FBXItemTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // --- 識別 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    EBXItemCategory Category = EBXItemCategory::Misc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    FGameplayTag SubCategoryTag;

    // --- 経済 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Economy")
    int32 BasePriceCredits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Economy", meta=(ClampMin="0.0", ClampMax="1.0"))
    float PriceVarianceRatio = 0.0f;

    // --- 重量 / スタック ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Stack")
    float UnitWeightKg = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Stack")
    int32 MaxStackCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Stack")
    bool bStackable = false;

    // --- 耐久 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Durability")
    bool bUsesDurability = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Durability")
    float DurabilityMax = 0.0f;

    // --- 使用条件 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Rules")
    bool bIsQuestItem = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Rules")
    bool bIsTradeable = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Rules")
    bool bCanStoreAtMerchant = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Rules")
    bool bCanBringIntoRaid = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Rules")
    EBXInventorySlotType InventorySlotType = EBXInventorySlotType::Backpack;

    // --- アセット参照 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UTexture2D> IconTexture;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UStaticMesh> WorldMesh;

    // --- UI / メタ ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item")
    FName UseActionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Collector")
    FName CollectorSeriesId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Item|Story")
    FName StoryFlagRequired;
};
