\# 明日やること(2026-04-27 以降)



\## Sprint 11 実行(最優先)



`docs/SPRINT\_11\_PLAN.md` を Claude Code に投げて Sprint 11 を実行する。



\### Sprint 11 の目的

§23-4 プレイヤー基底クラス `APlayerCharacterBase` を C++ で実装し、

PIE で歩き回れる状態に向けた第一歩を作る。



\### 起動手順

1\. Claude Code を Cursor または terminal で起動

2\. `docs/SPRINT\_11\_PLAN.md` の中の「Claude Code への指示文」セクションを丸ごとコピペ

3\. 実行を見守る(必要に応じて対話で軌道修正)



\## SPEC 統合状況(完了済み、参考)



2026-04-26 夜に ChatGPT で取得した 17 章を SPEC\_PartB.md に統合済み:

\- §8-2, §14-6, §16-4

\- §23-4, §23-5, §23-6, §23-7

\- §24-1, §24-2, §24-3

\- §29-3 詳細版, §29-4 詳細版, §29-5

\- §31-1, §31-2, §31-3, §31-4



最新 commit: eec0a24 (docs: SPEC\_PartB §23 系・§24 系・§29 詳細版・§31 系 SPEC 追記)



\## UE5 エディタ作業(累積中、優先度中)



Sprint 11 の C++ 実装後、UE5 エディタで以下を実行:



\- \[ ] エディタを開いて Sprint 11 のビルドを反映

\- \[ ] BP\_BX\_Player の Blueprint 派生を作成(APlayerCharacterBase から)

\- \[ ] 入力アクション IA\_BX\_\* のアセット作成(24 個)

\- \[ ] Input Mapping Context 作成と BP\_BX\_Player への割当

\- \[ ] BP\_BX\_Player をデフォルト Pawn に設定(GM\_BX\_Raid)

\- \[ ] PIE で歩き回れることを確認



\## 後続 Sprint 候補



\- Sprint 12: §23-6 HealthBodyParts コンポーネント実装(7 部位 HP)

\- Sprint 13: §23-7 PlayerInteraction コンポーネント実装

\- Sprint 14: §23-5 WeaponHandler コンポーネント実装

\- Sprint 15: §24-3 MedicalUseSubsystem 実装



\## DT 動作検証(優先度中)



\- \[ ] DT\_BX\_Medical の PIE 検証

\- \[ ] DT\_BX\_Merchants の PIE 検証

\- \[ ] DT\_BX\_Quests の PIE 検証

\- \[ ] DT\_Extract\_KansaiIndustrial01 の PIE 検証



\## 重要参照



\- 朝のオリジナル commit: 406ba09(SPRINT\_REPORT 追記)

\- DT 5 個追加 commit: 0a5990c

\- SPEC 統合 commit: eec0a24

\- ChatGPT セッション保存: docs/TEMP\_chatgpt\_all.md (1.17 MB, .gitignore 対象外)

\- 朝のバックアップ: docs/SPEC\_PartB\_backup\_now.md (.gitignore 対象外)

\- 統合前バックアップ: docs/SPEC\_PartB\_backup\_before\_merge2.md (.gitignore 対象外)



\## ChatGPT セッション続編候補



明日以降 ChatGPT に取得依頼する候補章:

\- §14-5 ダメージ計算モデル(既存にあるが詳細化要件あり)

\- §15-2 状態異常 DataTable と Neuro Critical 詳細化

\- §32-1 商人ハブの全体システム設計

\- §33 系 AI / NPC 関連

\- §34 系 サウンド / 音響伝播

