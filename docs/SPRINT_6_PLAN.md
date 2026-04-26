# 実装計画: Sprint 6 — 医療アイテム詳細 (§15-3)

## 対象仕様
- SPEC_PartB.md §15-3: FBXMedicalItemRow / UBXMedicalUseSubsystem

## 変更ファイル

| 種別 | パス |
|---|---|
| 編集 | `Source/BX/Public/Data/BXEnums.h`（EBXMedicalType 追加） |
| 新規 | `Source/BX/Public/Data/Medical/FBXMedicalItemRow.h` |
| 新規 | `Source/BX/Public/Systems/Medical/BXMedicalUseSubsystem.h` |
| 新規 | `Source/BX/Private/Systems/Medical/BXMedicalUseSubsystem.cpp` |
| 新規 | `DataSource/medical.csv`（8行以上） |
| 新規 | `docs/SPRINT_6_PLAN.md`（本ファイル） |

## 設計方針

- `DT_BX_Items` の `ItemId` と 1:1 対応する別テーブル `DT_BX_MedicalItems`
- `CuresStatusIds`: TArray<FName> — CSV 書式: `"(status_a,status_b)"`
- `UBXMedicalUseSubsystem`: UGameInstanceSubsystem 派生
  - FTimerHandle で UseTimeSec を管理（タイマー完了時に効果適用）
  - TMap<FName, FTimerHandle> で CooldownSec を管理
  - 被弾 / ダッシュ / 武器使用時は BP からキャンセルメソッドを呼ぶ設計
- Neuro Critical 解除は AC_BX_StatusEffects::TreatStatus() 経由

## 注意事項

- TArray<FName> の CSV インポート書式が UE5 5.4 で `(a,b)` で動作するか要確認
  → 問題時は QUESTIONS.md Q-06 として追記
- EBXMedicalType は BXEnums.h への追加（既存 enum への破壊的変更なし）
