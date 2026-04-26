// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/MerchantHub/FBXMerchantTradeViewModels.h"

#include "BXMerchantTradeSubsystem.generated.h"

class UDataTable;
class UBXMerchantNetworkSubsystem;

// 商人取引画面サブシステム (SPEC §29-4)
// - 買い / 売り価格算出
// - 取引セッション状態管理
// - 数量変更に応じた ProjectedCredits 更新
UCLASS()
class BX_API UBXMerchantTradeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // DT_BX_Items への参照 (価格取得用)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|UI|Trade")
    TSoftObjectPtr<UDataTable> ItemDataTable;

    // --- セッション管理 ---

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void BeginTradeSession(FName MerchantId, int32 InitialPlayerCredits);

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void EndTradeSession();

    // --- 操作 ---

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void SetCurrentTab(EBXTradeTab Tab);

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void SelectItem(FName ItemId);

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void SetQuantity(int32 Quantity);

    // --- 価格算出 ---

    // 購入価格: BasePriceCredits * BuyPriceMultiplier * (1 + PriceVarianceRatio)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    int32 CalculateBuyPrice(FName ItemId) const;

    // 売却価格: BasePriceCredits * SellPriceMultiplier * DurabilityRatio
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    int32 CalculateSellPrice(FName ItemId, float DurabilityRatio = 1.0f) const;

    // --- 取引実行 ---

    // 現在の選択内容で購入を確定する (クレジット不足時は失敗)
    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    bool CommitBuy();

    // 現在の選択内容で売却を確定する
    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    bool CommitSell();

    // --- ViewModel クエリ ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    FBXMerchantTradeSessionViewModel GetSessionViewModel() const;

    // 商人販売品リストから取引行を構築する (外部から ItemIds を渡す)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    TArray<FBXMerchantTradeRowViewModel> BuildMerchantInventoryRows(
        const TArray<FName>& MerchantItemIds) const;

    // プレイヤー所持品から取引行を構築する (外部から ItemIds と量を渡す)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    TArray<FBXMerchantTradeRowViewModel> BuildPlayerInventoryRows(
        const TArray<FName>& OwnedItemIds, const TArray<int32>& Quantities) const;

    // --- クレジット同期 ---

    UFUNCTION(BlueprintCallable, Category="BX|UI|Trade")
    void SetPlayerCredits(int32 Credits);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|Trade")
    int32 GetPlayerCredits() const { return SessionVM.PlayerCurrentCredits; }

private:
    FBXMerchantTradeSessionViewModel SessionVM;
    bool bSessionActive = false;

    void UpdateProjectedCredits();
    int32 GetItemBasePrice(FName ItemId) const;
    float GetItemPriceVarianceRatio(FName ItemId) const;
    bool IsItemTradeable(FName ItemId) const;
    UBXMerchantNetworkSubsystem* GetMerchantNetworkSub() const;
};
