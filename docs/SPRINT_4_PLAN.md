# 実装計画: Sprint 4 — インベントリ基本構造 (§15-1)

## 対象仕様
- SPEC_PartB.md §15-1: FBXItemTableRow / FBXInventorySlotDef / FBXInventoryItemStack / AC_BX_Inventory

## Q-02 解決方針（QUESTIONS.md より）
> **FBXSavedItemStack を廃止し FBXInventoryItemStack に統合する**
> 理由: 別構造体を維持すると Save ↔ Runtime 同期バグの温床になる。
> FBXInventoryItemStack は §15-1 の全フィールドを持つため Save 用途も兼ねられる。

ユーザー指示では "Q-05" と記載されているが、QUESTIONS.md 上は Q-02 が該当。

## 変更ファイル

| 種別 | パス |
|---|---|
| 新規 | `Source/BX/Public/Items/FBXInventoryTypes.h` |
| 新規 | `Source/BX/Public/Data/FBXItemTableRow.h` |
| 新規 | `Source/BX/Public/Items/Components/AC_BX_Inventory.h` |
| 新規 | `Source/BX/Private/Items/Components/AC_BX_Inventory.cpp` |
| 新規 | `DataSource/items.csv`（25 行） |
| 編集 | `Source/BX/Public/Save/FBXMerchantNetworkState.h`（FBXSavedItemStack 廃止） |
| 編集 | `Source/BX/Public/Save/FBXRunSnapshot.h`（FBXSavedItemStack 廃止） |
| 編集 | `docs/QUESTIONS.md`（Q-02 クローズ） |
| 新規 | `docs/SPRINT_4_PLAN.md`（本ファイル） |

## 構造体の依存関係

```
FBXInventorySlotDef   ─┐
FBXInventoryItemStack ─┤← Items/FBXInventoryTypes.h（§15-1-1 / §15-1-2）
                        │
FBXItemTableRow  ←── Data/FBXItemTableRow.h（§15-1 DT 行）
                        │
AC_BX_Inventory  ←── Items/Components/AC_BX_Inventory.h
  └─ TArray<FBXInventoryItemStack> Items
  └─ TSoftObjectPtr<UDataTable> ItemDataTable

FBXMerchantNetworkState.h（Save/）
  └─ include Items/FBXInventoryTypes.h（FBXSavedItemStack を置換）

FBXRunSnapshot.h（Save/）
  └─ include Items/FBXInventoryTypes.h（直接 include に変更）
```

## AC_BX_Inventory の実装スコープ

| 関数 | 実装内容 |
|---|---|
| `AddItem` | スタック統合 + 新規スタック追加 |
| `RemoveItem` | スタック消費（後ろから）|
| `GetTotalWeightKg` | 全スタックの Quantity × UnitWeightKg 合算 |
| `CanAcceptItem` | SlotType 一致 + bCanBringIntoRaid チェック |
| `HasItem` | 指定数以上の所持確認 |
| `GetItemQuantity` | 全スタックの合計数 |
| `SerializeToArray` | Items をそのまま返す（Save 用） |
| `LoadFromArray` | Items を上書き（ロード用） |

## Blueprint 作業（ユーザー対応）

```
1. /Content/BX/Data/Common/Items/ を開く
2. 右クリック → Miscellaneous → Data Table
3. Row Structure: FBXItemTableRow → 名前: DT_BX_Items → Save
4. DT_BX_Items を開き Reimport → DataSource/items.csv を指定
5. Save All
```
