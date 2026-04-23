# TK001 環境セットアップ完全ガイド

BLACKOUT EXFIL を開発する TK001_DEV-MAIN(i7-13620H / RTX4070 / 32GB RAM / Windows)で、**Claude Pro 枠内だけ** で運用できる環境を構築する手順。

**所要時間の目安**: 3〜5時間(UE5 のダウンロード時間含む)

---

## ⚠️ このガイドを始める前に

- 管理者権限を持つ Windows アカウントで作業すること
- **SSD の空き容量 150GB 以上**(UE5 本体 100GB + プロジェクト50GB想定)
- 安定したネット回線(UE5 の初回DLで 30〜50GB 転送)
- Claude Pro に契約済みであること

---

# Stage 0: 既存ツールの確認

TK001 には既に多くのツールが入っているはず。まず何が揃っているかを確認する。

## 0-1. PowerShell を起動

- スタートメニュー → **PowerShell 7**(なければ Windows PowerShell でも可)
- 右クリック → **管理者として実行**

## 0-2. 既存ツールのバージョン確認

以下を1つずつ実行して、結果を確認する:

```powershell
# Git
git --version
# 期待: git version 2.40 以上

# Git LFS
git lfs --version
# 期待: git-lfs/3.x 以上

# Node.js
node --version
# 期待: v18.x 以上(Claude Code は v18+ 必要)

# npm
npm --version
# 期待: 9.x 以上

# Python
python --version
# (UE5 には直接不要だが、CSV加工等で使うことがある)

# PowerShell 自身
$PSVersionTable.PSVersion
# 期待: 7.x 推奨
```

## 0-3. 足りないものをチェック

| ツール | TK001 に既に入っている想定 | ない場合の対応 |
|---|---|---|
| Git | ✅ あり | [git-scm.com](https://git-scm.com/) |
| Git LFS | ⚠️ 要確認 | Git 本体と一緒にインストールされる場合が多いが、個別DLも可 |
| Node.js | ✅ あり | [nodejs.org](https://nodejs.org/)(v20 LTS 推奨) |
| PowerShell 7 | △ 7推奨 | `winget install Microsoft.PowerShell` |
| Visual Studio 2022 | ❌ 未導入の可能性高 | Stage 1 で導入 |
| Unreal Engine 5.4 | ❌ 未導入 | Stage 2 で導入 |
| Claude Code | ❌ 未導入 | Stage 3 で導入 |

Git LFS がなかったら:
```powershell
winget install GitHub.GitLFS
# または
# https://git-lfs.com/ から手動インストール
```

---

# Stage 1: Visual Studio 2022 の導入

UE5 の C++ ビルドには **Visual Studio 2022** が必要。Community 版で OK。

## 1-1. インストーラーをダウンロード

[Visual Studio 2022 Community](https://visualstudio.microsoft.com/ja/vs/community/) から `VisualStudioSetup.exe` を取得して実行。

## 1-2. ワークロードを選択

**重要**: 以下3つのワークロードを必ずチェック。

- ✅ **C++ によるデスクトップ開発**
- ✅ **C++ によるゲーム開発**(← **Unreal Engine インストーラー** が中に含まれる)
- ✅ **.NET デスクトップ開発**(`BX.Build.cs` 等のビルド用)

## 1-3. 個別コンポーネントで追加チェック

右のタブ **個別のコンポーネント** で以下を追加確認:

- ✅ MSVC v143 - VS 2022 C++ x64/x86 ビルドツール(最新版)
- ✅ Windows 11 SDK(最新)または Windows 10 SDK(最新)
- ✅ C++ AddressSanitizer
- ✅ .NET Framework 4.8 SDK

## 1-4. インストール

- インストール場所はデフォルト(`C:\Program Files\Microsoft Visual Studio\2022\Community`)で OK
- 所要時間: 30〜60分、ディスク使用量 30〜40GB

## 1-5. 確認

インストール完了後、PowerShell を**新しく開き直して**:

```powershell
# VS のインストーラが認識されているか
& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
# → C:\Program Files\Microsoft Visual Studio\2022\Community などが出ればOK
```

---

# Stage 2: Unreal Engine 5.4 の導入

## 2-1. Epic Games Launcher をインストール

[Epic Games Launcher ダウンロード](https://store.epicgames.com/ja/download) から `EpicInstaller.msi` を取得して実行。

インストール後、Epic Games アカウントでサインイン(無料)。

## 2-2. UE 5.4 をインストール

1. Launcher 起動 → 左メニュー **Unreal Engine** → **ライブラリ** タブ
2. **エンジンバージョン** 右の **+** ボタン
3. ドロップダウンで **5.4.x**(2026年4月時点の最新ホットフィックス)を選択
4. **インストール** クリック

### インストール場所

- デフォルト: `C:\Program Files\Epic Games\UE_5.4`
- SSD 空きが厳しいなら **別 SSD へ変更可**(例: `D:\Epic Games\UE_5.4`)

### インストールオプション

- ✅ Core Components(必須)
- ✅ Starter Content(推奨、外してもよい)
- ✅ Templates and Feature Packs
- ✅ Engine Source(C++ デバッグ用に推奨)
- ⚠️ Editor symbols for debugging(**巨大**なので不要なら外す。入れると +50GB)
- ✅ Platforms → Windows(必須)
  - 他プラットフォーム(Android / iOS / Linux)は**最初は外す**

所要時間: ネット回線次第で 30分〜2時間。

## 2-3. Source code editor を VS2022 に設定

UE5 エディタ起動後(まだプロジェクトは作らなくてOK):

- 編集メニュー → **エディタの環境設定** → **全般 - ソースコード**
- **Source Code Editor** を **Visual Studio 2022** に設定

---

# Stage 3: Claude Code を Pro 専用で導入

**最重要セクション**。ここを間違えると API 課金が発生する。

## 3-1. 環境変数 `ANTHROPIC_API_KEY` を確実に削除

**これが設定されたままだと Claude Code は Pro を使わず API 課金になる**。徹底的に確認する。

```powershell
# 3つのスコープ全部をチェック
Write-Host "--- Process scope ---"
echo $env:ANTHROPIC_API_KEY

Write-Host "--- User scope ---"
[Environment]::GetEnvironmentVariable("ANTHROPIC_API_KEY", "User")

Write-Host "--- Machine scope ---"
[Environment]::GetEnvironmentVariable("ANTHROPIC_API_KEY", "Machine")
```

**期待結果**: 3つとも**何も表示されない**(空または null)

もし何か表示されたら削除:

```powershell
# User スコープ削除
[Environment]::SetEnvironmentVariable("ANTHROPIC_API_KEY", $null, "User")

# Machine スコープ削除(管理者 PowerShell で)
[Environment]::SetEnvironmentVariable("ANTHROPIC_API_KEY", $null, "Machine")

# 現在のセッションからも消す
Remove-Item Env:\ANTHROPIC_API_KEY -ErrorAction SilentlyContinue
```

削除後は **PowerShell を閉じて開き直す** → もう一度確認して空であることを確認。

### 他の API 関連環境変数も念のため確認

```powershell
[Environment]::GetEnvironmentVariable("ANTHROPIC_AUTH_TOKEN", "User")
[Environment]::GetEnvironmentVariable("CLAUDE_API_KEY", "User")
# いずれも空であるべき
```

## 3-2. claude.ai で Extra Usage を OFF に

ブラウザで:

1. [claude.ai](https://claude.ai) にログイン(Pro アカウント)
2. 左下のアバター → **Settings** → **Usage**(または Billing)
3. **Extra Usage** / **追加使用量** の項目を探し、**OFF** になっていることを確認
4. もし ON なら OFF に切替

> Extra Usage が ON だと、Pro の制限を超えたら自動的に従量課金(API レート)に切り替わる。これを OFF にしておけば、制限に達した瞬間に止まる = 課金されない。

## 3-3. Claude Code を npm でインストール

```powershell
npm install -g @anthropic-ai/claude-code

# 確認
claude --version
# 期待: 何らかのバージョン番号が出る(例: 1.x.x)
```

### エラー対処

- `npm ERR! EACCES` → 管理者 PowerShell で再実行
- `claude: command not found` → PATH に npm global bin が入っていない。以下で確認:
  ```powershell
  npm config get prefix
  # 結果のパスに `\claude.cmd` があるはず
  # このパスが $env:Path に含まれていない場合は追加
  ```

## 3-4. Pro アカウントでログイン

```powershell
claude login
```

ブラウザが開くので:

1. **Claude Pro 契約中の同じメールアドレス**でサインイン
2. 「Claude Code に Claude アカウントへのアクセスを許可しますか?」→ **Allow / 許可**
3. ブラウザ側に「成功しました」表示 → ターミナルに戻る

### もし Console / PAYG の選択肢が出たら

「Claude Console (Pay-as-you-go)」と「Claude.ai subscription」のような選択肢が出る場合があります。**必ず Claude.ai subscription(Pro)側を選ぶ**。間違えて Console を選んだら:

```powershell
claude logout
claude login  # もう一度やり直し
```

## 3-5. Pro で認証されていることを確認

```powershell
# 適当なディレクトリで
cd $env:USERPROFILE
claude
```

起動したら **`/status`** と打つ:

- ✅ `Plan: Pro` と表示されれば成功
- ❌ `API credits` や `Pay-as-you-go` と表示されたら 3-1 の環境変数か 3-4 のログインに問題あり → やり直し

`/exit` または Ctrl+C 2回で抜ける。

---

# Stage 4: BLACKOUT EXFIL プロジェクト作成

## 4-1. 作業ディレクトリ準備

```powershell
# 開発用の親ディレクトリ(既に D:\Dev などがあればそこに)
mkdir D:\Dev -Force
cd D:\Dev
```

## 4-2. Epic Launcher から UE5 プロジェクト作成

1. Epic Launcher → UE 5.4 の **起動** ボタン
2. プロジェクトブラウザ:
   - カテゴリ: **ゲーム**
   - テンプレート: **ブランク**(**Blank**)※ FPS Template もあるが、後々独自クラス構成にするので Blank 推奨
3. **プロジェクトのデフォルト** セクション:
   - プロジェクトタイプ: **C++** を選択 ← **重要**
   - ターゲットプラットフォーム: **デスクトップ**
   - 品質プリセット: **最大**
   - レイトレーシング: **オフ**(後で有効化可。初期は OFF で軽く)
   - Starter Content: **OFF**(BX は独自アセット主体)
4. **プロジェクトの場所**: `D:\Dev`
5. **プロジェクト名**: `BX` ← ここ重要、仕様書の略称と一致
6. **作成** クリック

初回起動は **シェーダーコンパイルに10〜30分**かかる。コーヒーを淹れるタイミング。

完了すると `D:\Dev\BX\BX.uproject` と周辺ファイルができている。

## 4-3. エディタを一度閉じる

スキャフォールド配置中は閉じておく(ファイル競合防止)。

## 4-4. テンプレートを展開

`ue5_claude_template.zip` を適当な場所(例: `D:\Downloads`)に解凍:

```powershell
# zip のパスは各自のダウンロード先に合わせて
Expand-Archive -Path "$env:USERPROFILE\Downloads\ue5_claude_template.zip" -DestinationPath "$env:USERPROFILE\Downloads\ue5_claude_template_extracted" -Force
```

解凍された中身を `D:\Dev\BX\` に**上書きコピー**:

```powershell
# 中身を BX プロジェクトルートにコピー
Copy-Item -Path "$env:USERPROFILE\Downloads\ue5_claude_template_extracted\ue5_claude_template\*" -Destination "D:\Dev\BX\" -Recurse -Force
```

コピー後の `D:\Dev\BX\` はこうなっているはず:

```
D:\Dev\BX\
├── BX.uproject           ← UE5 が作った
├── BX.sln                ← UE5 が作った(VS プロジェクト)
├── Binaries/             ← UE5 が作った
├── Config/               ← UE5 が作った
├── Content/              ← UE5 が作った
├── Intermediate/         ← UE5 が作った
├── Saved/                ← UE5 が作った
├── Source/BX/            ← UE5 が作った
│
├── CLAUDE.md             ← ★ スキャフォールドから
├── README.md             ← ★
├── SETUP_GUIDE.md        ← ★
├── .gitignore            ← ★
├── .gitattributes        ← ★
├── docs/                 ← ★
├── scripts/              ← ★
└── DataSource/           ← ★
```

---

# Stage 5: Git と Git LFS の初期化

## 5-1. Git LFS 初期化(このマシンで1回だけ)

```powershell
git lfs install
# → Git LFS initialized. と表示されれば OK
```

## 5-2. リポジトリ初期化

```powershell
cd D:\Dev\BX

# Git 初期化
git init
git branch -M main

# ユーザー設定(まだしていなければ)
git config user.name "takumi2911"
git config user.email "your.email@example.com"   # ← 自分のメールに置換
```

## 5-3. GitHub リポジトリ作成とリモート追加

GitHub で先に **`BX`** というプライベートリポジトリを作成(README・license・gitignore は**空のまま**作る)。

```powershell
git remote add origin git@github.com:takumi2911/BX.git
# または HTTPS なら
# git remote add origin https://github.com/takumi2911/BX.git
```

## 5-4. 初回コミット(.gitattributes を必ず先に)

**順番が重要**。`.gitattributes` を先に commit しないと LFS が効かない。

```powershell
# 1. .gitattributes と .gitignore を先に commit
git add .gitattributes .gitignore
git commit -m "chore: add git attributes and ignore rules"

# 2. ドキュメント類を commit
git add CLAUDE.md README.md SETUP_GUIDE.md docs/ scripts/ DataSource/
git commit -m "docs: add project scaffold and conventions"

# 3. UE5 生成物を commit(.uasset は LFS に乗る)
git add .
git commit -m "chore: initial UE5 project files"

# 4. push
git push -u origin main
```

LFS に乗っているか確認:

```powershell
git lfs ls-files
# → .uasset ファイルがあれば一覧に出る
```

---

# Stage 6: 動作確認

## 6-1. 環境チェックスクリプト実行

```powershell
cd D:\Dev\BX
.\scripts\setup-check.ps1
```

すべて **OK** (緑)になっていれば合格。1つでも赤があれば該当 Stage に戻る。

## 6-2. UE5 C++ ビルドの確認

1. `D:\Dev\BX\BX.uproject` を右クリック → **Generate Visual Studio project files**
   - 右クリックメニューに出ない場合: PowerShell で
     ```powershell
     & "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="D:\Dev\BX\BX.uproject" -game -rocket -progress
     ```
2. `BX.sln` を Visual Studio 2022 で開く
3. 上部のドロップダウンを **Development Editor / Win64** に
4. メニュー: **ビルド → ソリューションのビルド**(または F7)
5. **0 エラー 成功** で終われば OK(ワーニングは出ても OK)

## 6-3. UE5 からプロジェクト起動

`BX.uproject` をダブルクリック、または Epic Launcher → 最近のプロジェクトから起動。

起動したら:
- 右下 **Live Coding Status** が緑になっていることを確認
- Viewport が表示されることを確認
- プレイ ▶ ボタンで Play in Editor(PIE)が動くか確認

## 6-4. Claude Code 動作確認

PowerShell で:

```powershell
cd D:\Dev\BX
claude
```

最初のメッセージで:

> CLAUDE.md を読んで、このプロジェクトの方針を3行で要約して

と入力。BLACKOUT EXFIL / UE5.4 / Blueprint+C++ 等のキーワードが含まれた要約が返ってくれば、CLAUDE.md の読み込みも含めて全部動いている。

---

# Stage 7: 日常運用の設定(任意)

## 7-1. TK003 への自動バックアップ設定(既存構成と統合する場合)

TK003 のバックアップ機構(robocopy + Task Scheduler)の対象に `D:\Dev\BX` を追加する。例えば TK003 側で:

```powershell
# TK003 の PowerShell(既存のバックアップ基盤に追加)
robocopy "\\TK001\Dev\BX" "D:\Backups\BX" /MIR /XD Binaries Intermediate Saved DerivedDataCache /XF *.sln /R:2 /W:5 /LOG+:D:\Logs\bx_backup.log
```

`Binaries`, `Intermediate`, `Saved`, `DerivedDataCache` は除外対象(再生成可・巨大)。

## 7-2. VS Code 統合(エディタとして使う場合)

```powershell
# BX プロジェクトを VS Code で開く
code D:\Dev\BX
```

推奨拡張機能:
- **Unreal Engine Assistant**(シンタックス補助)
- **C/C++**(Microsoft 公式)
- **GitLens**
- **Markdown All in One**(SPEC.md 編集用)

ただし C++ ビルドは VS2022 側で行うほうが UE5 との相性が良い。

---

# トラブルシュート

| 症状 | 原因 | 対処 |
|---|---|---|
| `claude --version` が効かない | npm global PATH 未通し | `npm config get prefix` のパスを `$env:Path` に追加 |
| `/status` で API credits 表示 | 環境変数 or ログイン間違い | Stage 3-1 の環境変数削除 → `claude logout` → `claude login` |
| UE5 起動時に「ソースビルド修復」プロンプト | 初回 or VS2022 未検出 | **はい**を選ぶ → 5〜15分待つ |
| `Generate VS project files` 右クリックに出ない | UE5 シェル統合が未登録 | UE5 エディタの 編集 → エディタの環境設定 → ソースコード → **Refresh Visual Studio Project** を一度実行 |
| ビルドで `error MSB8020` | VS ビルドツールのバージョン不一致 | VS Installer で MSVC v143 最新版を確認 |
| `.uasset` が LFS に乗らない | `.gitattributes` が後で追加された | `git lfs migrate import --include="*.uasset,*.umap" --everything` |
| `claude` セッション中に突然 API credits に切り替わる | Extra Usage が ON | claude.ai → Settings → Usage で OFF |

---

# セットアップ完了後の次アクション

ここまで終われば、以下の「最初の依頼」を Claude Code に投げる準備が整っている:

```
> git status
(クリーンな状態)

> claude
```

最初の実装依頼例:

> CLAUDE.md と docs/SPEC.md を読みました。§23-3 Phase 0 の作業のうち、
> - Source/BX/Public/Core/ に空の GI_BX / SG_BX / GM_BX 基底 C++ クラス
> - Source/BX/Public/Data/ に FBXWeaponTableRow の雛形(G-02 の定義を反映)
> - DataSource/weapons.csv の雛形(§30-5 の価格帯で5件)
> を追加してください。BX.Build.cs の依存モジュールも必要なら更新を。

この依頼は Phase 0 の成果物がそのまま出てくる粒度。ここから始めればスムーズに実装フェーズに入れる。

---

**最終チェックリスト**

- [ ] Stage 0: PowerShell で各ツールのバージョン確認完了
- [ ] Stage 1: VS2022 + C++ ゲーム開発ワークロードで導入
- [ ] Stage 2: UE 5.4 インストール、ソースエディタを VS2022 に設定
- [ ] Stage 3-1: ANTHROPIC_API_KEY が3スコープすべて空
- [ ] Stage 3-2: claude.ai で Extra Usage が OFF
- [ ] Stage 3-5: `claude` → `/status` で `Plan: Pro` 表示
- [ ] Stage 4: `D:\Dev\BX` に UE5 プロジェクト作成、スキャフォールド配置
- [ ] Stage 5: Git LFS 初期化、GitHub へ push 完了
- [ ] Stage 6-2: VS2022 で `BX.sln` が **0 エラー** でビルド
- [ ] Stage 6-4: Claude Code が CLAUDE.md を正しく読めている

全部チェックが付いたら環境構築完了。
