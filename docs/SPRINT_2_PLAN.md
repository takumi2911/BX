# 実装計画: Sprint 2 — 武器 / 弾薬 DataTable 行構造

## 対象仕様
- SPEC_PartB.md §14-4: `FBXWeaponTableRow` 全フィールド
- SPEC_PartB.md §14-5-2: `FBXAmmoTableRow` 代表弾 11 種

## 変更ファイル

| 種別 | パス |
|---|---|
| 新規 | `Source/BX/Public/Data/FBXWeaponTableRow.h` |
| 新規 | `Source/BX/Public/Data/FBXAmmoTableRow.h` |
| 新規 | `DataSource/weapons.csv` (サンプル 3 行) |
| 新規 | `DataSource/ammo.csv` (§14-5-2 代表弾 11 種) |
| 編集 | `Source/BX/BX.Build.cs` (Niagara モジュール追加) |

## 設計判断

- `FBXWeaponTableRow` は `FTableRowBase` 派生 USTRUCT、`BlueprintType` 付与
- `UNiagaraSystem` を参照するため Build.cs に `"Niagara"` を追加
- `USkeletalMesh / USoundBase / UAnimMontage / UCurveFloat / UTexture2D` は forward declaration で対応（ヘッダ依存を最小化）
- `FGameplayTagContainer` / `FGameplayTag` は GameplayTags モジュール（既存）から include
- `FireModes` は `TArray<EBXFireMode>`（CSV では `|` 区切り文字列でインポート）
- CSV の列名は C++ フィールド名と完全一致（UE5 DataTable インポート要件）
- `FBXAmmoTableRow` は Niagara / アセット参照なし → Niagara 依存は武器側のみ

## Blueprint 作業（ユーザー対応）

実装完了後、UE5 エディタで以下を実施してください:

```
1. /Content/BX/Data/Common/Weapons/ を開く
2. 右クリック → Miscellaneous → Data Table
3. Row Structure: FBXWeaponTableRow → 名前: DT_BX_Weapons → Save
4. DT_BX_Weapons を開き Reimport → DataSource/weapons.csv を指定
5. /Content/BX/Data/Common/Ammo/ を開く（フォルダ新規作成）
6. 同様に FBXAmmoTableRow で DT_BX_Ammo を作成 → DataSource/ammo.csv をインポート
7. Save All
```

## ビルド結果
- (実装後に記載)
