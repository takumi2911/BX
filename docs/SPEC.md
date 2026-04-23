# BLACKOUT EXFIL 仕様書

## 分冊構成

- **Part A**(ゲーム設計・世界観・フェーズ計画): 今後 ChatGPT と追記予定
- **Part B**(Claude Code 実装仕様): `docs/SPEC_PartB.md` を参照

## Part B 収録章(実装優先)

- §14-4 武器 DataTable 行構造
- §14-5 ダメージ計算モデル
- §15-1 アイテム DT とインベントリ基本
- §15-2 状態異常 DT と Neuro Critical
- §15-3 医療アイテム詳細
- §16-3 セーブ構造体フィールド詳細
- §18-2 抽出条件データ構造
- §18-3 抽出ポイント Actor 基底
- §21-1 商人 DT と Trust 解放
- §21-2 依頼 DT と進行構造
- §29-3 商人ハブホーム画面
- §29-4 商人ハブ取引画面

## Claude Code への指示ルール

- 実装依頼は `§X-Y` の章番号で指定する(Part B のどの章か)
- 仕様書中の `ENUM_BX_*` は C++ では `EBX*` に読み替える(CLAUDE.md §4-2 準拠)
- 部位ダメージは**7部位**前提(Head / Chest / Abdomen / LeftArm / RightArm / LeftLeg / RightLeg)
- 既存の ENUM_BX_StatusType 等の新規 Enum は BXEnums.h へ追加する

## 経緯

詳しい現状把握は `docs/HANDOFF.md` を参照。
