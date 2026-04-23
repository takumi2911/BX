// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Items/Components/AC_BX_Inventory.h"
#include "Data/FBXItemTableRow.h"
#include "Engine/DataTable.h"

UAC_BX_Inventory::UAC_BX_Inventory()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// -----------------------------------------------------------------------
// Private helper
// -----------------------------------------------------------------------

const FBXItemTableRow* UAC_BX_Inventory::FindItemRow(FName ItemId) const
{
    UDataTable* DT = ItemDataTable.Get();
    if (!DT || ItemId.IsNone()) return nullptr;
    return DT->FindRow<FBXItemTableRow>(ItemId, TEXT("AC_BX_Inventory"));
}

// -----------------------------------------------------------------------
// AddItem — スタック統合 → 新規スタック追加
// -----------------------------------------------------------------------

bool UAC_BX_Inventory::AddItem(FName ItemId, int32 Quantity, bool bFoundInRaid)
{
    if (ItemId.IsNone() || Quantity <= 0) return false;

    const FBXItemTableRow* Row = FindItemRow(ItemId);
    const int32 MaxStack = Row ? Row->MaxStackCount : 1;
    const bool bStackable = Row ? Row->bStackable : false;
    const float DurMax = (Row && Row->bUsesDurability) ? Row->DurabilityMax : 100.0f;

    int32 Remaining = Quantity;

    // 既存スタックへの積み増し
    if (bStackable)
    {
        for (FBXInventoryItemStack& Stack : Items)
        {
            if (Stack.ItemId != ItemId || Stack.Quantity >= MaxStack) continue;
            const int32 ToAdd = FMath::Min(Remaining, MaxStack - Stack.Quantity);
            Stack.Quantity += ToAdd;
            Remaining -= ToAdd;
            if (Remaining <= 0) return true;
        }
    }

    // 新規スタック生成
    while (Remaining > 0)
    {
        FBXInventoryItemStack NewStack;
        NewStack.ItemId = ItemId;
        NewStack.Quantity = FMath::Min(Remaining, MaxStack);
        NewStack.CurrentDurability = DurMax;
        NewStack.bIsFoundInRaid = bFoundInRaid;
        Items.Add(NewStack);
        Remaining -= NewStack.Quantity;
    }

    return true;
}

// -----------------------------------------------------------------------
// RemoveItem — 後ろのスタックから消費
// -----------------------------------------------------------------------

bool UAC_BX_Inventory::RemoveItem(FName ItemId, int32 Quantity)
{
    if (ItemId.IsNone() || Quantity <= 0) return false;
    if (GetItemQuantity(ItemId) < Quantity) return false;

    int32 Remaining = Quantity;
    for (int32 i = Items.Num() - 1; i >= 0 && Remaining > 0; --i)
    {
        if (Items[i].ItemId != ItemId) continue;
        const int32 ToRemove = FMath::Min(Remaining, Items[i].Quantity);
        Items[i].Quantity -= ToRemove;
        Remaining -= ToRemove;
        if (Items[i].Quantity <= 0)
        {
            Items.RemoveAt(i);
        }
    }
    return Remaining == 0;
}

// -----------------------------------------------------------------------
// クエリ
// -----------------------------------------------------------------------

float UAC_BX_Inventory::GetTotalWeightKg() const
{
    UDataTable* DT = ItemDataTable.Get();
    float Total = 0.0f;
    for (const FBXInventoryItemStack& Stack : Items)
    {
        const FBXItemTableRow* Row = DT ? DT->FindRow<FBXItemTableRow>(Stack.ItemId, TEXT("GetWeight")) : nullptr;
        const float UnitWeight = Row ? Row->UnitWeightKg : 0.0f;
        Total += UnitWeight * Stack.Quantity;
    }
    return Total;
}

bool UAC_BX_Inventory::IsOverWeight() const
{
    return GetTotalWeightKg() > MaxWeightKg;
}

bool UAC_BX_Inventory::CanAcceptItem(FName ItemId, EBXInventorySlotType SlotType) const
{
    const FBXItemTableRow* Row = FindItemRow(ItemId);
    if (!Row) return true; // DT 未ロード時は楽観的に許可

    if (!Row->bCanBringIntoRaid) return false;
    return Row->InventorySlotType == SlotType;
}

bool UAC_BX_Inventory::HasItem(FName ItemId, int32 RequiredQuantity) const
{
    return GetItemQuantity(ItemId) >= RequiredQuantity;
}

int32 UAC_BX_Inventory::GetItemQuantity(FName ItemId) const
{
    int32 Total = 0;
    for (const FBXInventoryItemStack& Stack : Items)
    {
        if (Stack.ItemId == ItemId) Total += Stack.Quantity;
    }
    return Total;
}

// -----------------------------------------------------------------------
// Save / Load
// -----------------------------------------------------------------------

TArray<FBXInventoryItemStack> UAC_BX_Inventory::SerializeToArray() const
{
    return Items;
}

void UAC_BX_Inventory::LoadFromArray(const TArray<FBXInventoryItemStack>& InItems)
{
    Items = InItems;
}
