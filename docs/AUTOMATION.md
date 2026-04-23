# 自動化ガイド

ステップ4(UE5 エディタ作業 → ビルド → 動作確認 → commit)を**どこまで自動化できるか**、TK001 で実際に使える形でまとめました。

---

## 結論: 完全自動化は無理、半自動化で 70% 削減可能

以下の4分類で作業を整理すると、自動化の可否が明確になります。

| 作業分類 | 自動化可否 | 手段 |
|---|---|---|
| **A. C++ / CSV / Config 編集** | ✅ 完全自動 | Claude Code |
| **B. Blueprint / DataTable / DataAsset のアセット**新規作成** | ✅ 自動化可 | UE5 Python |
| **C. ビルド + 成功時の commit** | ✅ 完全自動 | PowerShell |
| **D. Blueprint グラフ内部・Widget レイアウト・Material 編集** | ❌ 不可 | 人間が UE5 エディタで作業 |
| **E. 動作確認(PIE で実際に動かす)** | ❌ 不可 | 人間が目視 |

**結論**: D と E は人間が必要。ただし D は **C++ で吸収できる範囲を広げる**ことで減らせる。

---

## 提供されるスクリプト

`scripts/` に以下を同梱しています。

### 1. `scripts/build_and_commit.ps1`

C++ 変更後の **ビルド → Git commit** を 1 コマンドで実行。

```powershell
# 基本
.\scripts\build_and_commit.ps1 -Message "feat: §14-4 武器 DT 行構造を実装"

# push まで
.\scripts\build_and_commit.ps1 -Message "feat: ..." -AutoPush

# コミットせずビルドのみ
.\scripts\build_and_commit.ps1 -SkipCommit

# ビルドせず commit のみ(ドキュメント変更時)
.\scripts\build_and_commit.ps1 -Message "docs: ..." -SkipBuild
```

内部処理:
1. VS プロジェクトファイル再生成
2. MSBuild で Development Editor / Win64 ビルド
3. `git status` で変更確認
4. ビルド成功時のみ `git add -A && git commit -m ...`
5. `-AutoPush` 指定時は push

**ビルドエラーで止まる**ので、エラーメッセージを Claude Code に渡して修正してもらえます。

### 2. `scripts/ue5_python/create_blueprints_batch.py`

C++ 基底クラスから Blueprint を**一括作成**。

UE5 エディタの Python Console で実行:
- `BP_BX_Player`(APlayerCharacterBase 派生)
- `BP_BX_AI_Looter`(AEnemyCharacterBase 派生)
- `BP_BX_Weapon_Pistol9mm`(AWeaponBase 派生)
- ...等、スクリプト内のリスト分を一括生成

これで**「BP を派生してリネーム」のマウス作業がゼロに**なります。

ただし **BP グラフ内部のノード編集はできません**。生成された BP を開いて、プロパティのデフォルト値設定や追加コンポーネント配置は人間の作業。

### 3. `scripts/ue5_python/reimport_datatables.py`

CSV を更新したら DT を自動 Reimport。

```
DataSource/weapons.csv を Claude Code で更新
  ↓
UE5 エディタで reimport_datatables.py を実行
  ↓
DT_BX_Weapons が最新 CSV で再インポートされる
```

### 4. `scripts/watch_csv.ps1`

`DataSource/*.csv` を監視し、変更を自動検知する雛形。

**注意**: 実運用には UE5 の Remote Execution API 設定が必要(エディタ側で Python Remote Execution を ON)。まずは手動実行に慣れてから導入推奨。

---

## 現実的な運用フロー(半自動化版)

```
[Claude Code]
  §23-4 Phase 1 プレイヤー操作を C++ で実装
    ├── Source/BX/Public/Characters/PlayerCharacterBase.h 作成
    ├── Source/BX/Private/Characters/PlayerCharacterBase.cpp 作成
    ├── BX.Build.cs に "EnhancedInput" 追加
    └── DataSource/weapons.csv に 5 件追加
          ↓
[TK001 PowerShell]
  .\scripts\build_and_commit.ps1 -Message "feat: §23-4 プレイヤー操作基底"
  → ビルド成功 → commit
          ↓
[UE5 エディタ]
  Python Console で create_blueprints_batch.py 実行
  → BP_BX_Player が自動生成される
          ↓
[UE5 エディタ(人間作業)]
  BP_BX_Player を開いて:
  - Mesh に SK_BX_Player を設定
  - CameraComponent を追加
  - IMC_BX_Default を設定
  → Save
          ↓
[UE5 エディタ(人間作業)]
  Play in Editor で動作確認
          ↓
[TK001 PowerShell]
  .\scripts\build_and_commit.ps1 -Message "feat: BP_BX_Player assets"
  → .uasset 差分を commit
```

**この流れで「1機能を動く状態まで持っていく」時間が、従来の 1/3 〜 1/2 になります。**

---

## 完全自動化したい気持ちへの現実的な回答

「クリック1つで全部終わる」を目指す気持ちはわかりますが、以下の理由で推奨しません:

**1. 壊れたときの復旧コストが異常に高い**
Blueprint のノード配置を RPA で自動化すると、UE5 バージョン更新・解像度変更・ウィンドウ位置変更で即死します。TK001 が動作不能になるリスクに見合いません。

**2. 判断が必要な瞬間が必ず来る**
「ビルドエラーは無視していい警告か、直すべきエラーか」「動作確認で違和感があったら仕様の問題か実装の問題か」— これらは AI に完全委任すると品質が壊滅します。

**3. 今の構成は既に十分速い**
Claude Code が C++/CSV を生成 + Python スクリプトで BP 量産 + PowerShell でビルド&commit。残る手作業は「BP グラフ編集」「PIE で動作確認」だけ。これは**人間がやるべき仕事**です(目視の品質判断を AI に投げてはいけない)。

---

## 本当に何度もやる繰り返し作業が残ったら

もし「同じ BP 編集パターンを 20 回やっている」と気づいたら、そのパターンは**C++ で吸収すべき**という合図です。Claude Code に「この編集パターンを C++ の基底クラスに取り込んで、BP 派生時のデフォルト値にしたい」と依頼すれば、以降は BP での手作業がほぼゼロになります。

これが **「C++ で土台、Blueprint で調整」** という方針の本質です。繰り返しが増えてきたら C++ 側に責務を移す。これが最良の自動化戦略です。

---

## まとめ

- **ステップ4 の自動化率は 70% まで引き上げ可能**(スクリプト3本で実現)
- **残り 30%(BP グラフ編集・PIE 動作確認)は人間の仕事**として受け入れる
- **繰り返し作業が増えたら C++ 側に責務を移す** ことで、手作業を継続的に減らしていく

「全自動」への憧れより、「**ミスが発生しにくい・復旧が早い半自動**」のほうが、個人開発では圧倒的に継続しやすいです。
