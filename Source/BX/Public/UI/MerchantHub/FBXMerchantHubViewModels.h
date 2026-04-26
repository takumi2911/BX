// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UTexture2D;

#include "FBXMerchantHubViewModels.generated.h"

// §29-3-2 商人一覧行 ViewModel
USTRUCT(BlueprintType)
struct BX_API FBXMerchantHubMerchantRowViewModel
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FName MerchantId;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 TrustLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    bool bHasNewQuest = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    bool bHasUrgentWarning = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    TSoftObjectPtr<UTexture2D> IconTexture;
};

// §29-3-1 ホーム画面 ViewModel
USTRUCT(BlueprintType)
struct BX_API FBXMerchantHubHomeViewModel
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FName MerchantId;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FText MerchantDisplayName;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FText MerchantTagline;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 TrustLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 TrustPoints = 0;

    // 次レベルまで必要な残りポイント (max レベルなら 0)
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 TrustPointsToNext = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 StoredInventoryCount = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 UnlockedRespawnNodeCount = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 ActiveQuestCount = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    int32 PlayerCredits = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FName CurrentRegionId;

    // 最近の変動ログ (最大 5 行)
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    TArray<FText> RecentLogLines;

    // 不足物資 / 重傷 / 期限警告など
    UPROPERTY(BlueprintReadOnly, Category="BX|UI|MerchantHub")
    FGameplayTagContainer WarningTags;
};
