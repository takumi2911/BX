// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

class UTexture2D;

#include "FBXMerchantTradeViewModels.generated.h"

// §29-4-1 取引行 ViewModel
USTRUCT(BlueprintType)
struct BX_API FBXMerchantTradeRowViewModel
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    FName ItemId;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 QuantityOwned = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 UnitPriceCredits = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 TotalPriceCredits = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    bool bCanBuy = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    bool bCanSell = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    bool bInsufficientCredits = false;

    // 0.0〜1.0
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float DurabilityRatio = 1.0f;

    // 装備比較差分 (武器のみ)
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float CompareDeltaDamage = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float CompareDeltaWeightKg = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float CompareDeltaADSTimeSec = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    TSoftObjectPtr<UTexture2D> IconTexture;
};

// §29-4-2 取引セッション ViewModel
USTRUCT(BlueprintType)
struct BX_API FBXMerchantTradeSessionViewModel
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    FName MerchantId;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 PlayerCurrentCredits = 0;

    // 選択数量での取引後想定クレジット
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 ProjectedCreditsAfterTrade = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    EBXTradeTab CurrentTab = EBXTradeTab::Buy;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    FName SelectedItemId;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    int32 SelectedQuantity = 1;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float MerchantBuyPriceMultiplier = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    float MerchantSellPriceMultiplier = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    FText TransactionWarningText;

    // 最近の取引ログ (最大 5 件)
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|Trade")
    TArray<FText> RecentTradeLog;
};
