# 明日やること(2026-04-27 以降)

## ChatGPT 出力の整理(今夜取得した 16 章)
docs/TEMP_chatgpt_all.md に ChatGPT セッション全文がある。
以下の 16 章を抽出・整形してから docs/SPEC_PartB.md に追記。

### 取得済み章
- §8-2 ダメージ状態異常
- §14-6 防具 DataTable
- §16-4 参照構造体詳細
- §23-4 プレイヤー基底クラス
- §23-5 武器保持コンポーネント
- §23-6 BodyPart/Health (Chest 採用版)
- §23-7 Interaction/Loot
- §24-1 インベントリコンポーネント
- §24-2 アイテム DataTable
- §24-3 医療アイテム使用仕様
- §29-3 商人ハブホーム UI
- §29-4 商人個別画面 UI
- §29-5 商人売買フロー
- §31-1 脱出ポイント基底
- §31-2 KansaiIndustrial01 脱出一覧
- §31-3 KansaiIndustrial01 POI/スポーン/導線
- §31-4 KansaiIndustrial01 ルート/鍵/電源/依頼配置

## SPEC_PartB.md とのダブり判定
VS Code で Ctrl+F 検索:
- §29-3, §29-4 → 既存にあり、内容比較必要
- §23-4 → ヒット 1 件あり、章タイトルか言及かチェック
- その他 → ヒットなしなら追記

## 統合方針
- 被らない章: そのまま追記
- 被る章: 新版で置換 or 既存と統合判断
- commit: "docs: §23-4 系・§24-3・§29-3〜5・§31 系 SPEC 追記 (ChatGPT)"

## Sprint 11 準備
- §23-4 プレイヤー基底クラスを Claude Code で実装
- "歩いて撃てる" 状態に到達する Sprint
- 必要な新規 enum:
  EBXViewMode, EBXLocomotionState, EBXLeanState, EBXCombatState
  EBXWeaponSlot, EBXReloadType
  EBXInteractTargetType, EBXInventoryTransferResult
  EBXMedicalInterruptReason, EBXMedicalUseFailReason
  EBXExtractFailReason, EBXExtractUiState
  EBXItemUseActionType, EBXItemRarity, EBXCarryState, EBXLanguage
  EBXMerchantHubState, EBXMerchantDetailTab, EBXMerchantTransactionType
  EBXLootContainerType, EBXKeySpawnMode, EBXQuestObjectType
  EBXArmorCategory, EBXArmorMaterialType, EBXArmorEquipSlot

## UE5 エディタ作業(累積中、優先度中)
- DT 動作検証(Sprint 6-9 の DT を PIE で確認)
- 抽出ポイント Actor の Blueprint 派生 x 6
- 商人ハブ Widget Blueprint 派生
- 商人 5 体の DataAsset 作成

## 重要参照
- 朝のオリジナル commit: 406ba09(SPRINT_REPORT 追記)
- 最新 commit: 0a5990c(DT 5 つ追加)
- ChatGPT セッション保存: docs/TEMP_chatgpt_all.md (.gitignore 対象外)
- 朝のバックアップ: docs/SPEC_PartB_backup_now.md (.gitignore 対象外)
