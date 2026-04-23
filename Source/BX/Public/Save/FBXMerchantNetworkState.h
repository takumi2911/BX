// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FBXMerchantNetworkState.generated.h"

// セーブ用アイテムスタック（ランタイム FBXInventoryItemStack の保存版）
// 完全定義は §15-1 実装後に FBXInventoryItemStack と統合予定 → docs/QUESTIONS.md Q-02
USTRUCT(BlueprintType)
struct BX_API FBXSavedItemStack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    float CurrentDurability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Inventory")
    bool bIsFoundInRaid = false;
};

// 商人 1 人分の状態 (SPEC_GAPS G-01)
USTRUCT(BlueprintType)
struct BX_API FBXMerchantState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    int32 TrustLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    int32 TrustPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FBXSavedItemStack> StoredInventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FName> ActiveQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FName> CompletedQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FName> FailedQuests;
};

// 商人ネットワーク全体の状態 (SPEC_GAPS G-01 / CLAUDE.md §6-2)
USTRUCT(BlueprintType)
struct BX_API FBXMerchantNetworkState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    int32 SaveVersion = 1;

    // MerchantId → 個別状態
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TMap<FName, FBXMerchantState> MerchantStates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FName> UnlockedRespawnNodes;
};
