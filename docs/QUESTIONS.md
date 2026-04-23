# 未確定仕様の質問リスト

実装中に仕様書（SPEC_PartB.md / SPEC_GAPS.md）に記載のなかった項目。
ChatGPT と §16-3 を整備する際に確認・追記してください。

---

## Q-01 FBXGlobalSettings — キーコンフィグ構造

**背景**: CLAUDE.md §6-2 に「キーコンフィグ」とあるが、具体的なデータ型が未定義。

**候補**:
- A: `TMap<FName, FKey>` でアクション名 → キーをマップ
- B: Enhanced Input の `UInputMappingContext` を `TSoftObjectPtr` で参照し、セーブは差分のみ
- C: §16-3 で別構造体として定義

**現在の実装**: キーコンフィグフィールドは省略。`FBXGlobalSettings` は音量と感度のみ。

---

## Q-02 FBXSavedItemStack — §15-1 の FBXInventoryItemStack との統合

**背景**: §15-1 で `FBXInventoryItemStack`（ランタイム用スタック）が定義されているが、
セーブ用の型として別途 `FBXSavedItemStack` を `FBXMerchantNetworkState.h` 内に定義した。

**確認事項**:
- §15-1 実装時に `FBXInventoryItemStack` を整備したら、`FBXSavedItemStack` と統合するか、
  別型として共存させるか（セーブ軽量化の観点から別型のほうが望ましい場合がある）

---

## Q-03 FBXPlayerProgress — 依頼進行フィールド

**背景**: CLAUDE.md §6-2 に「依頼進行」とあるが、SPEC_GAPS.md G-01 には FBXPlayerProgress の
フィールド詳細が記載されていない。

**候補フィールド**:
- `ActiveQuestIds: TArray<FName>` — 進行中依頼 ID 一覧（商人ごとではなくグローバル管理が望ましいか）
- `CompletedQuestIds: TArray<FName>` — 完了依頼

**確認事項**: 依頼進行は `FBXMerchantState.ActiveQuests` で管理するか、
`FBXPlayerProgress` でグローバル管理するか、両方か。

---

## Q-04 FBXRegionState — イベント状態の詳細

**背景**: CLAUDE.md §6-2 に「地域フラグ, イベント状態」とあるが、
具体的なフィールドが §16-3 に未記載。

**確認事項**:
- 地域ごとの「安全度」「封鎖状態」「敵増援フラグ」等が必要か
- `TMap<FName, FBXRegionState>` で RegionStates を保持する現行設計で問題ないか

---

## Q-05 FBXRunSnapshot — 中断セーブの発動条件

**背景**: CLAUDE.md §6-2 に「レイド中断セーブ（条件付き）」とあるが、
どの条件（プレイヤーが任意中断 / 緊急終了 / 商人モードのみ等）で発生するか未定義。

**確認事項**:
- EXリアルモードでは中断セーブ禁止？
- 商人預け保持モードでのみ有効？
- プレイヤーの任意中断（ゲーム終了）を許容するか

---

## 次のアクション

上記 Q-01〜Q-05 を ChatGPT と §16-3 の執筆時に確認し、
SPEC_PartB.md に §16-3 として追記後、Sprint 3 の実装を拡張してください。
