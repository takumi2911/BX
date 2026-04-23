# Claude Code への最終提出文（BLACKOUT EXFIL）

以下をそのまま Claude Code に渡してください。

---

あなたはこれから、Unreal Engine 5.4 で開発するオフライン専用高難易度脱出 FPS
**BLACKOUT EXFIL (BX)** の実装支援を行います。

## 参照資料
最優先で読むファイル:
- `BLACKOUT_EXFIL_full_spec_v2.md`

必要に応じて参照するファイル:
- `BLACKOUT_EXFIL_full_spec_v2.zip`

## この仕様書の扱い
- `BLACKOUT_EXFIL_full_spec_v2.md` を**実装のソースオブトゥルース**として扱ってください。
- 章番号（例: `§14-4`, `§14-5`, `§16-3`）は固定です。勝手に番号変更しないでください。
- 命名規則、接頭辞、フォルダ構成は**仕様書の記載を厳守**してください。
- エンジンは **UE 5.4 固定**です。5.5 以降前提の API や実装は使わないでください。
- アセットの `.uasset` を直接編集する前提ではなく、**C++ / CSV / 設定 / 手順書**中心で進めてください。

## 絶対に変えない前提
- プロジェクトコードネーム: **BX**
- エンジン: **UE 5.4**
- モード: **オフライン専用**
- 2モード構成:
  - リアルハードコア
  - 商人預け保持ハードコア
- 初期地域: **KansaiIndustrial01**
- 商人 5人:
  - ミナト
  - サワベ
  - クロガネ
  - ヨシナガ
  - アマギ
- 部位ダメージ: **7部位固定**
  - Head
  - Thorax
  - Abdomen
  - LeftArm
  - RightArm
  - LeftLeg
  - RightLeg
- 武器カテゴリ:
  - HG
  - SMG
  - AR
  - SG
  - DMR
  - SR

## 実装の優先順
まずは**縦切りで動く最小構成**を作ってください。最初から全章を同時に広げないでください。

### Phase 1: 仕様読解と実装計画
1. `BLACKOUT_EXFIL_full_spec_v2.md` を読む
2. 実装対象を章番号付きで分解する
3. 依存関係を整理する
4. 最初の着手順を提示する

### Phase 2: 最優先で着手する章
以下を最優先で実装してください。

1. `§14-4 武器 DataTable の行構造詳細`
2. `§14-5 ダメージ計算モデル`
3. `§16-3 セーブ構造体フィールド詳細`

### Phase 3: 最初に作る成果物
最初の作業では、少なくとも以下を作成してください。

#### C++ / 構造体
- `FBXWeaponTableRow`
- `SG_BX_Profile`
- `FBXProfileMeta`
- `FBXGlobalSettings`
- `FBXMerchantNetworkState`
- `FBXMerchantState`
- `FBXPlayerProgress`
- `FBXRegionState`
- `FBXRunSnapshot`

#### DataTable / CSV ひな形
- `DT_BX_Weapons`
- `DT_BX_Ammo`
- 必要なら `DataSource/` 配下の CSV ソース

#### システム基盤
- ダメージ計算ユーティリティ
- 防具貫通判定
- 部位別ダメージ反映
- Save / Load 基盤
- SaveVersion を用いた移行余地

## 出力ルール
- 仕様書の章番号を引用しながら進めてください
- 新規追加したクラス、CSV、ファイルは**フルパス**で示してください
- できない作業（uasset 直接編集など）は、**人間が UE エディタで行う手順**として明示してください
- 不明点があれば、仕様書のどこが曖昧かを章番号付きで列挙してください
- 勝手に大きな仕様変更をしないでください

## 実装時の判断ルール
- **C++**:
  - `FTableRowBase` 派生
  - `USaveGame`
  - 計算処理
  - Subsystem
  - 基底クラス
- **Blueprint**:
  - UI
  - 画面遷移
  - 軽量な表示制御
  - 演出
- **C++ 基底 + BP 派生**:
  - Actor / Character / GameMode / UI 連携対象

## 最初の返答フォーマット
Claude Code は、まず次の形式で返してください。

1. **仕様理解サマリ**
2. **実装優先順位**
3. **最初に作るファイル一覧**
4. **不明点 / 要確認点**
5. **着手開始**

## 補足
- 仕様が不足している場合でも、章番号付きで不足点を示してから進めてください
- 命名規則や接頭辞は仕様書通りにしてください
- 日本語と英語の両対応を前提にしてください
- 将来の拡張よりも、まずは `KansaiIndustrial01` の縦切り成立を優先してください

---
