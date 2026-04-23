// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/FBXInventoryTypes.h"

#include "FBXMerchantNetworkState.generated.h"

// 商人 1 人分の状態 (SPEC_GAPS G-01)
// StoredInventory は FBXInventoryItemStack（§15-1-2）で統一 — Sprint 4 で FBXSavedItemStack を廃止
USTRUCT(BlueprintType)
struct BX_API FBXMerchantState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    int32 TrustLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    int32 TrustPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Merchant")
    TArray<FBXInventoryItemStack> StoredInventory;

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
