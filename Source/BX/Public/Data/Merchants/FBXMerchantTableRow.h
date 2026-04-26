// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

class UTexture2D;
class UUserWidget;

#include "FBXMerchantTableRow.generated.h"

// DT_BX_Merchants 行構造体 — 商人マスタ (SPEC §21-1-1)
USTRUCT(BlueprintType)
struct BX_API FBXMerchantTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName MerchantId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    EBXMerchantType MerchantType = EBXMerchantType::Trader;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName HomeRegionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FText Tagline;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Trust")
    int32 InitialTrustLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Trust")
    int32 InitialTrustPoints = 0;

    // Trust Lv1→Lv2 到達に必要な累計ポイント
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Trust")
    int32 Trust2Threshold = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Trust")
    int32 Trust3Threshold = 260;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Trust")
    int32 Trust4Threshold = 520;

    // プレイヤーが買う時の価格補正 (1.0 = 定価)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Economy")
    float BuyPriceMultiplier = 1.0f;

    // プレイヤーが売る時の価格補正 (1.0 = 定価)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Economy")
    float SellPriceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UTexture2D> PortraitTexture;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftClassPtr<UUserWidget> MerchantHubWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName DefaultTradePoolId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName DefaultQuestPoolId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName DefaultRespawnPoolId;

    // NAME_None なら接触制限なし
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant")
    FName StoryFlagRequiredForContact;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Service")
    bool bCanUnlockRespawnNodes = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Service")
    bool bCanProvideMedicalService = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Merchant|Service")
    bool bCanProvideTravelService = false;
};
