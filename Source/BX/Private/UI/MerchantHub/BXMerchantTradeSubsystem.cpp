// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "UI/MerchantHub/BXMerchantTradeSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/FBXItemTableRow.h"
#include "Data/Merchants/FBXMerchantTableRow.h"
#include "Systems/Merchant/BXMerchantNetworkSubsystem.h"

void UBXMerchantTradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

// --- セッション管理 ---

void UBXMerchantTradeSubsystem::BeginTradeSession(FName MerchantId, int32 InitialPlayerCredits)
{
    SessionVM = FBXMerchantTradeSessionViewModel();
    SessionVM.MerchantId            = MerchantId;
    SessionVM.PlayerCurrentCredits  = InitialPlayerCredits;
    SessionVM.ProjectedCreditsAfterTrade = InitialPlayerCredits;
    SessionVM.CurrentTab            = EBXTradeTab::Buy;
    SessionVM.SelectedQuantity      = 1;

    UBXMerchantNetworkSubsystem* NetSub = GetMerchantNetworkSub();
    UDataTable* MerchantDT = nullptr;
    // Price multipliers are stored on the merchant DataTable; we query via NetSub's MerchantDataTable.
    if (NetSub)
    {
        UDataTable* DT = NetSub->MerchantDataTable.Get();
        if (DT)
        {
            const FBXMerchantTableRow* Row = DT->FindRow<FBXMerchantTableRow>(MerchantId, TEXT("BeginTradeSession"));
            if (Row)
            {
                SessionVM.MerchantBuyPriceMultiplier  = Row->BuyPriceMultiplier;
                SessionVM.MerchantSellPriceMultiplier = Row->SellPriceMultiplier;
            }
        }
    }

    bSessionActive = true;
}

void UBXMerchantTradeSubsystem::EndTradeSession()
{
    bSessionActive = false;
    SessionVM      = FBXMerchantTradeSessionViewModel();
}

// --- 操作 ---

void UBXMerchantTradeSubsystem::SetCurrentTab(EBXTradeTab Tab)
{
    SessionVM.CurrentTab = Tab;
    UpdateProjectedCredits();
}

void UBXMerchantTradeSubsystem::SelectItem(FName ItemId)
{
    SessionVM.SelectedItemId  = ItemId;
    SessionVM.SelectedQuantity = 1;
    UpdateProjectedCredits();
}

void UBXMerchantTradeSubsystem::SetQuantity(int32 Quantity)
{
    SessionVM.SelectedQuantity = FMath::Max(1, Quantity);
    UpdateProjectedCredits();
}

// --- 価格算出 ---

int32 UBXMerchantTradeSubsystem::CalculateBuyPrice(FName ItemId) const
{
    const int32 Base     = GetItemBasePrice(ItemId);
    const float Variance = GetItemPriceVarianceRatio(ItemId);
    const float Multi    = SessionVM.MerchantBuyPriceMultiplier;
    return FMath::RoundToInt(static_cast<float>(Base) * Multi * (1.0f + Variance));
}

int32 UBXMerchantTradeSubsystem::CalculateSellPrice(FName ItemId, float DurabilityRatio) const
{
    const int32 Base  = GetItemBasePrice(ItemId);
    const float Multi = SessionVM.MerchantSellPriceMultiplier;
    const float Clamp = FMath::Clamp(DurabilityRatio, 0.0f, 1.0f);
    return FMath::RoundToInt(static_cast<float>(Base) * Multi * Clamp);
}

// --- 取引実行 ---

bool UBXMerchantTradeSubsystem::CommitBuy()
{
    if (!bSessionActive || SessionVM.SelectedItemId.IsNone())
    {
        return false;
    }

    const int32 TotalCost = CalculateBuyPrice(SessionVM.SelectedItemId) * SessionVM.SelectedQuantity;
    if (SessionVM.PlayerCurrentCredits < TotalCost)
    {
        SessionVM.TransactionWarningText = FText::FromString(TEXT("クレジット不足"));
        return false;
    }

    SessionVM.PlayerCurrentCredits -= TotalCost;
    SessionVM.RecentTradeLog.Insert(
        FText::Format(FText::FromString(TEXT("購入: {0} x{1} -{2}C")),
            FText::FromName(SessionVM.SelectedItemId),
            FText::AsNumber(SessionVM.SelectedQuantity),
            FText::AsNumber(TotalCost)),
        0);
    if (SessionVM.RecentTradeLog.Num() > 5)
    {
        SessionVM.RecentTradeLog.SetNum(5);
    }
    SessionVM.TransactionWarningText = FText::GetEmpty();
    UpdateProjectedCredits();
    return true;
}

bool UBXMerchantTradeSubsystem::CommitSell()
{
    if (!bSessionActive || SessionVM.SelectedItemId.IsNone())
    {
        return false;
    }

    if (!IsItemTradeable(SessionVM.SelectedItemId))
    {
        SessionVM.TransactionWarningText = FText::FromString(TEXT("このアイテムは売却不可"));
        return false;
    }

    const int32 TotalGain = CalculateSellPrice(SessionVM.SelectedItemId) * SessionVM.SelectedQuantity;
    SessionVM.PlayerCurrentCredits += TotalGain;
    SessionVM.RecentTradeLog.Insert(
        FText::Format(FText::FromString(TEXT("売却: {0} x{1} +{2}C")),
            FText::FromName(SessionVM.SelectedItemId),
            FText::AsNumber(SessionVM.SelectedQuantity),
            FText::AsNumber(TotalGain)),
        0);
    if (SessionVM.RecentTradeLog.Num() > 5)
    {
        SessionVM.RecentTradeLog.SetNum(5);
    }
    SessionVM.TransactionWarningText = FText::GetEmpty();
    UpdateProjectedCredits();
    return true;
}

// --- ViewModel クエリ ---

FBXMerchantTradeSessionViewModel UBXMerchantTradeSubsystem::GetSessionViewModel() const
{
    return SessionVM;
}

TArray<FBXMerchantTradeRowViewModel> UBXMerchantTradeSubsystem::BuildMerchantInventoryRows(
    const TArray<FName>& MerchantItemIds) const
{
    TArray<FBXMerchantTradeRowViewModel> Result;
    UDataTable* DT = ItemDataTable.Get();
    if (!DT)
    {
        return Result;
    }

    for (const FName& ItemId : MerchantItemIds)
    {
        const FBXItemTableRow* Row = DT->FindRow<FBXItemTableRow>(ItemId, TEXT("BuildMerchantInventoryRows"));
        if (!Row)
        {
            continue;
        }

        FBXMerchantTradeRowViewModel RowVM;
        RowVM.ItemId           = ItemId;
        RowVM.DisplayName      = Row->DisplayName;
        RowVM.QuantityOwned    = 0;
        RowVM.UnitPriceCredits = CalculateBuyPrice(ItemId);
        RowVM.TotalPriceCredits = RowVM.UnitPriceCredits;
        RowVM.bCanBuy          = Row->bIsTradeable && (SessionVM.PlayerCurrentCredits >= RowVM.UnitPriceCredits);
        RowVM.bCanSell         = false;
        RowVM.bInsufficientCredits = !RowVM.bCanBuy && Row->bIsTradeable;
        RowVM.DurabilityRatio  = 1.0f;
        RowVM.IconTexture      = Row->IconTexture;

        Result.Add(RowVM);
    }

    return Result;
}

TArray<FBXMerchantTradeRowViewModel> UBXMerchantTradeSubsystem::BuildPlayerInventoryRows(
    const TArray<FName>& OwnedItemIds, const TArray<int32>& Quantities) const
{
    TArray<FBXMerchantTradeRowViewModel> Result;
    UDataTable* DT = ItemDataTable.Get();
    if (!DT)
    {
        return Result;
    }

    const int32 Count = FMath::Min(OwnedItemIds.Num(), Quantities.Num());
    for (int32 i = 0; i < Count; ++i)
    {
        const FName& ItemId = OwnedItemIds[i];
        const FBXItemTableRow* Row = DT->FindRow<FBXItemTableRow>(ItemId, TEXT("BuildPlayerInventoryRows"));
        if (!Row)
        {
            continue;
        }

        FBXMerchantTradeRowViewModel RowVM;
        RowVM.ItemId           = ItemId;
        RowVM.DisplayName      = Row->DisplayName;
        RowVM.QuantityOwned    = Quantities[i];
        RowVM.UnitPriceCredits = CalculateSellPrice(ItemId);
        RowVM.TotalPriceCredits = RowVM.UnitPriceCredits * Quantities[i];
        RowVM.bCanBuy          = false;
        RowVM.bCanSell         = Row->bIsTradeable && (Quantities[i] > 0);
        RowVM.bInsufficientCredits = false;
        RowVM.DurabilityRatio  = 1.0f;
        RowVM.IconTexture      = Row->IconTexture;

        Result.Add(RowVM);
    }

    return Result;
}

// --- クレジット同期 ---

void UBXMerchantTradeSubsystem::SetPlayerCredits(int32 Credits)
{
    SessionVM.PlayerCurrentCredits = Credits;
    UpdateProjectedCredits();
}

// --- Private ---

void UBXMerchantTradeSubsystem::UpdateProjectedCredits()
{
    if (SessionVM.SelectedItemId.IsNone())
    {
        SessionVM.ProjectedCreditsAfterTrade = SessionVM.PlayerCurrentCredits;
        return;
    }

    if (SessionVM.CurrentTab == EBXTradeTab::Buy)
    {
        const int32 Cost = CalculateBuyPrice(SessionVM.SelectedItemId) * SessionVM.SelectedQuantity;
        SessionVM.ProjectedCreditsAfterTrade = SessionVM.PlayerCurrentCredits - Cost;
    }
    else
    {
        const int32 Gain = CalculateSellPrice(SessionVM.SelectedItemId) * SessionVM.SelectedQuantity;
        SessionVM.ProjectedCreditsAfterTrade = SessionVM.PlayerCurrentCredits + Gain;
    }
}

int32 UBXMerchantTradeSubsystem::GetItemBasePrice(FName ItemId) const
{
    UDataTable* DT = ItemDataTable.Get();
    if (!DT)
    {
        return 0;
    }
    const FBXItemTableRow* Row = DT->FindRow<FBXItemTableRow>(ItemId, TEXT("GetItemBasePrice"));
    return Row ? Row->BasePriceCredits : 0;
}

float UBXMerchantTradeSubsystem::GetItemPriceVarianceRatio(FName ItemId) const
{
    UDataTable* DT = ItemDataTable.Get();
    if (!DT)
    {
        return 0.0f;
    }
    const FBXItemTableRow* Row = DT->FindRow<FBXItemTableRow>(ItemId, TEXT("GetItemPriceVarianceRatio"));
    return Row ? Row->PriceVarianceRatio : 0.0f;
}

bool UBXMerchantTradeSubsystem::IsItemTradeable(FName ItemId) const
{
    UDataTable* DT = ItemDataTable.Get();
    if (!DT)
    {
        return false;
    }
    const FBXItemTableRow* Row = DT->FindRow<FBXItemTableRow>(ItemId, TEXT("IsItemTradeable"));
    return Row ? Row->bIsTradeable : false;
}

UBXMerchantNetworkSubsystem* UBXMerchantTradeSubsystem::GetMerchantNetworkSub() const
{
    UGameInstance* GI = GetGameInstance();
    return GI ? GI->GetSubsystem<UBXMerchantNetworkSubsystem>() : nullptr;
}
