// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/FBXInventoryTypes.h"
#include "Data/BXEnums.h"

class UDataTable;

#include "AC_BX_Inventory.generated.h"

// インベントリ管理コンポーネント (SPEC §15-1)
// 容量計算・スタック統合・重量再計算・スロット可否判定・Save シリアライズを担当
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_Inventory : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_Inventory();

    // --- 設定 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Inventory", meta=(ClampMin="0.0"))
    float MaxWeightKg = 24.0f;

    // DT_BX_Items への参照（重量・スタック上限の取得に使用）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Inventory")
    TSoftObjectPtr<UDataTable> ItemDataTable;

    // --- 現在の所持アイテム ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Inventory")
    TArray<FBXInventoryItemStack> Items;

    // --- アイテム操作 ---

    // アイテムを追加。スタック可能なら既存スタックに積む。重量制限は非チェック（呼び出し側で確認）
    UFUNCTION(BlueprintCallable, Category="BX|Inventory")
    bool AddItem(FName ItemId, int32 Quantity, bool bFoundInRaid = false);

    // アイテムを指定数消費。後ろのスタックから優先消費
    UFUNCTION(BlueprintCallable, Category="BX|Inventory")
    bool RemoveItem(FName ItemId, int32 Quantity);

    // --- クエリ ---

    UFUNCTION(BlueprintPure, Category="BX|Inventory")
    float GetTotalWeightKg() const;

    UFUNCTION(BlueprintPure, Category="BX|Inventory")
    bool IsOverWeight() const;

    // スロット種別と bCanBringIntoRaid を検証
    UFUNCTION(BlueprintPure, Category="BX|Inventory")
    bool CanAcceptItem(FName ItemId, EBXInventorySlotType SlotType) const;

    UFUNCTION(BlueprintPure, Category="BX|Inventory")
    bool HasItem(FName ItemId, int32 RequiredQuantity = 1) const;

    UFUNCTION(BlueprintPure, Category="BX|Inventory")
    int32 GetItemQuantity(FName ItemId) const;

    // --- Save / Load ---

    UFUNCTION(BlueprintCallable, Category="BX|Inventory")
    TArray<FBXInventoryItemStack> SerializeToArray() const;

    UFUNCTION(BlueprintCallable, Category="BX|Inventory")
    void LoadFromArray(const TArray<FBXInventoryItemStack>& InItems);

private:
    // DataTable 行を取得するヘルパー（DT 未ロード時は nullptr）
    const struct FBXItemTableRow* FindItemRow(FName ItemId) const;
};
