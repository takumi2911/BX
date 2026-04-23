# TK001 PC セットアップ手順書

**対象機**: TK001_DEV-MAIN (i7-13620H / RTX4070 / 32GB RAM / Windows 11)
**目的**: Unreal Engine 5.4 + Claude Code (Pro 枠内) で BLACKOUT EXFIL を開発できる状態にする
**所要時間目安**: 3〜4時間(ダウンロード時間含む、大半は待ち時間)

---

## 全体フロー

```
[Phase A] 事前チェック(15分)
   │
   ▼
[Phase B] インストール(2〜3時間、待ち時間多め)
   ├─ B-1. Git LFS
   ├─ B-2. Visual Studio 2022 + C++ ゲーム開発ワークロード
   ├─ B-3. Epic Games Launcher → UE 5.4
   └─ B-4. Claude Code
   │
   ▼
[Phase C] 設定(20分)
   ├─ C-1. Windows 長いパス対応
   ├─ C-2. 環境変数チェック(ANTHROPIC_API_KEY 不在確認)
   ├─ C-3. VS2022 を UE5 のデフォルトエディタに
   └─ C-4. Git 初期設定
   │
   ▼
[Phase D] 動作確認(30分)
   ├─ D-1. setup-check.ps1 実行
   ├─ D-2. Claude Code /status 確認
   └─ D-3. UE5 プロジェクト新規作成テスト
```

---

# Phase A: 事前チェック

## A-1. 既存ツールのバージョン確認

**PowerShell 7** を管理者権限で開いて実行:

```powershell
Write-Host "=== 既存ツール確認 ===" -ForegroundColor Cyan
git --version
node --version
npm --version
winget --version
# VS Code
code --version 2>$null

# オプション: C++ コンパイラがあるか
where.exe cl.exe 2>$null
```

### 期待される状態

| ツール | 必要版 | メモ |
|---|---|---|
| Git | 2.40+ | ✅ 既にあり |
| Node.js | **18.0+ 必須**(Claude Code 要件) | メモリによれば既にあり、版確認を |
| npm | Node に付属 | ✅ |
| winget | 1.0+ | Windows 11 には標準 |

**Node.js が 18 未満だった場合**:
```powershell
winget install -e --id OpenJS.NodeJS.LTS
# PowerShell 再起動して確認
```

## A-2. ディスク空き容量の確認

```powershell
Get-PSDrive -PSProvider FileSystem | Select-Object Name, @{Name="Free(GB)";Expression={[math]::Round($_.Free/1GB,1)}}, @{Name="Used(GB)";Expression={[math]::Round($_.Used/1GB,1)}}
```

### 必要空き容量(最低ライン)

- **UE5 エンジン本体**: ~100 GB
- **VS2022 + C++ ワークロード**: ~15 GB
- **UE5 プロジェクト作業領域**: 50 GB+
- **Epic Games Launcher**: ~2 GB
- **合計推奨**: **200 GB 以上の空き**(SSD 上)

※ D ドライブなど作業用 SSD に配置推奨。C ドライブは圧迫しやすい。

## A-3. 開発ディレクトリの決定

```powershell
# 例: D:\Dev\BX を作業ディレクトリに
mkdir D:\Dev\BX -ErrorAction SilentlyContinue

# Epic Games Launcher と UE エンジンのインストール先候補(あとで指定)
# 推奨: D:\Epic\  配下
mkdir D:\Epic -ErrorAction SilentlyContinue
```

## A-4. Windows 設定チェック

### Windows のバージョン
```powershell
winver  # Windows 11 22H2 以降が望ましい(UE5.4 の最新対応)
```

### 開発者モード(推奨)
`設定` → `システム` → `開発者向け` → **開発者モード ON**
(シンボリックリンク許可、後々プラグイン開発時に効いてくる)

---

# Phase B: インストール

## B-1. Git LFS(2分)

UE5 の `.uasset` / `.umap` バイナリを Git で扱うのに必須。

```powershell
# インストール
winget install -e --id GitHub.GitLFS

# PowerShell を一度閉じて再起動してから確認
git lfs --version

# グローバル初期化(マシンで1回だけ)
git lfs install
```

## B-2. Visual Studio 2022(60〜90分)

### B-2-1. Community Edition インストール

```powershell
winget install -e --id Microsoft.VisualStudio.2022.Community
```

または手動で https://visualstudio.microsoft.com/ja/vs/community/ からダウンロード。

### B-2-2. ワークロードの追加(重要)

インストール完了後、**Visual Studio Installer** を起動 → 2022 Community の **変更** ボタン → 以下にチェック:

#### ワークロードタブ

- ✅ **C++ によるデスクトップ開発**
- ✅ **C++ によるゲーム開発**(これに UE5 用ツール一式が含まれる)
- ✅ **.NET デスクトップ開発**(UE5 の `.Build.cs` は C# で書かれているため)

#### 個別コンポーネントタブ(追加でチェック)

- ✅ **MSVC v143 - VS 2022 C++ x64/x86 ビルドツール(最新)**
- ✅ **Windows 11 SDK**(最新版)
- ✅ **C++ プロファイリング ツール**
- ✅ **Windows 用 C++ AddressSanitizer**
- ✅ **Unreal Engine インストーラー**(ゲーム開発ワークロードに自動で含まれるはず)
- ✅ **.NET 6.0 ランタイム**

**変更** ボタンを押すと、追加ダウンロード ~10GB + インストール 20〜30分。

### B-2-3. 再起動

インストール完了後、**Windows を再起動**。環境変数とシェル統合が正しく効くのに必要。

## B-3. Epic Games Launcher & UE 5.4(60〜120分)

### B-3-1. Epic Games Launcher

```powershell
winget install -e --id EpicGames.EpicGamesLauncher
```

または https://store.epicgames.com/ja/download

### B-3-2. Epic Games アカウント作成 / ログイン

起動して、Epic Games アカウントでログイン(未作成なら新規作成)。

### B-3-3. UE 5.4 のインストール

1. Launcher 上部 → **Unreal Engine** タブ
2. 左メニュー → **ライブラリ**
3. **エンジンバージョン** 横の **+** ボタン
4. ドロップダウンから **5.4.x** を選択(5.5 / 5.6 は選ばない)
5. **インストール** ボタン → **参照** でインストール先を変更
   - 推奨: `D:\Epic\UE_5.4`
6. インストール中の「オプション コンポーネント」:
   - ✅ **Editor シンボル(デバッグ用)**
   - ✅ **エンジン ソース**(C++ デバッグ時にあると便利)
   - ✅ **ターゲット プラットフォーム > Windows**
   - ❌ Android / iOS / Linux / macOS(BX は PC 専用なので不要)
   - ❌ HoloLens(不要)

ダウンロード 50GB+ / 展開込みで最終 ~100GB。所要時間は回線次第で 1〜2時間。

## B-4. Claude Code(5分)

### B-4-1. インストール

```powershell
npm install -g @anthropic-ai/claude-code
```

### B-4-2. バージョン確認

```powershell
claude --version
```

**⚠️ ここでログインはまだしない**。Phase C-2 で環境変数を確認してから、課金トリガーを回避した状態で初回ログインする。

---

# Phase C: 設定

## C-1. Windows 長いパスの有効化(必須)

UE5 プロジェクトはネストが深く、Windows 標準の 260 文字パス上限に引っかかりやすい。

```powershell
# 管理者 PowerShell で実行
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
  -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force

# Git 側も有効化
git config --system core.longpaths true
```

再起動で反映される。

## C-2. 環境変数チェック(課金防止の最重要ステップ)

`ANTHROPIC_API_KEY` が設定されていると、Claude Code は Pro サブスクではなく **API 従量課金** でビリングされる。必ず削除する。

```powershell
# 3スコープ全部確認(ユーザー / マシン / 現セッション)
Write-Host "=== 環境変数チェック ===" -ForegroundColor Cyan
Write-Host "User scope:    $([Environment]::GetEnvironmentVariable('ANTHROPIC_API_KEY','User'))"
Write-Host "Machine scope: $([Environment]::GetEnvironmentVariable('ANTHROPIC_API_KEY','Machine'))"
Write-Host "Process scope: $env:ANTHROPIC_API_KEY"
```

**3つとも空(なにも表示されない)** のが正解。

### もし値が表示されたら削除

```powershell
# User スコープで設定されていた場合
[Environment]::SetEnvironmentVariable("ANTHROPIC_API_KEY", $null, "User")

# Machine スコープで設定されていた場合(管理者 PowerShell が必要)
[Environment]::SetEnvironmentVariable("ANTHROPIC_API_KEY", $null, "Machine")

# 現在のセッション
Remove-Item Env:\ANTHROPIC_API_KEY -ErrorAction SilentlyContinue
```

削除後 PowerShell を閉じて再起動、もう一度チェック。3つとも空になっていれば OK。

## C-3. VS2022 を UE5 のデフォルトエディタに

1. Epic Games Launcher から UE5.4 を初回起動
2. プロジェクトブラウザで **新規プロジェクト**(テスト用、あとで削除可)
3. エディタ起動後 → メニュー **編集** → **エディタの環境設定**
4. 左メニュー **一般** → **ソース コード** → **ソース コード エディタ**
5. ドロップダウンで **Visual Studio 2022** を選択

## C-4. Git グローバル設定

```powershell
git config --global user.name "takumi2911"
git config --global user.email "<GitHub登録メール>"
git config --global init.defaultBranch main
git config --global core.autocrlf false    # 改行コードを勝手に変換させない
git config --global core.longpaths true
git config --global pull.rebase true       # merge commit を増やさない
```

---

# Phase D: 動作確認

## D-1. setup-check.ps1 実行

スキャフォールドの検証スクリプトを走らせる:

```powershell
cd D:\Dev\BX  # ここに前回の zip を解凍して配置した前提
.\scripts\setup-check.ps1
```

**全項目 OK** になることを確認。NG があれば該当 Phase に戻る。

## D-2. Claude Code の初回ログイン & /status 確認

### ログイン

```powershell
claude
```

初回はブラウザが自動で開く → **Claude Pro 契約中のメールアドレス**でログイン → 認証完了。

**⚠️ 「Claude Console」「Pay-as-you-go」「API credits」などが選択肢に出ても選ばない**。必ず Pro / subscription 側を選ぶ。

### /status で確認

ログイン後、Claude Code が起動したら以下を打つ:

```
/status
```

表示内容で以下をチェック:

- ✅ **Plan: Pro** (または Pro subscription) と出る
- ❌ 「API credits」「pay-as-you-go」が主になっていない
- 現在の残量とリセット時刻が表示される

Plan が "Pro" になっていれば、この時点で **追加課金が発生しない構成が完成** している。

### 簡単なテスト

```
「CLAUDE.md を読んで、このプロジェクトの方針を3行で要約して」
```

正しく CLAUDE.md が読まれて要約が返ってくれば完了。

## D-3. UE5 プロジェクト作成テスト

Epic Launcher → UE 5.4 起動 → プロジェクトブラウザ:

- カテゴリ: **ゲーム**
- テンプレート: **First Person**
- プロジェクト設定:
  - **C++** を選択(最重要)
  - Starter Content: OFF
  - Raytracing: OFF(あとで有効化可)
  - ブループリント / C++: **C++**
  - 品質プリセット: Maximum
- 保存先: `D:\Dev\`
- プロジェクト名: `TestProject01`(動作確認用。成功したら削除)
- **作成**

初回起動で VS2022 が自動で開き、C++ コンパイルが走る(5〜15分)。
エディタが起動してデフォルトレベルが表示されれば成功。

確認後、このテストプロジェクトは削除して OK。

---

# Phase E: 本プロジェクト開始(参考)

D フェーズまで終わったら、本番の `BX` プロジェクトを作る:

1. Epic Launcher → UE5.4 起動 → **C++ First Person** テンプレート
2. 保存先: `D:\Dev\`、プロジェクト名: **BX**(短いほうがパス長有利)
3. 作成完了後、UE5 を一度閉じる
4. `D:\Dev\BX\` に、前回の `ue5_claude_template.zip` の中身をマージコピー
   (`CLAUDE.md`, `README.md`, `.gitignore`, `.gitattributes`, `docs/`, `DataSource/`, `scripts/`, `SETUP_GUIDE.md`)
5. PowerShell で:

```powershell
cd D:\Dev\BX
git init
git branch -M main
git lfs install
git add .gitattributes .gitignore
git commit -m "chore: configure Git LFS and ignore rules"
git add .
git commit -m "chore: initial UE5 project scaffold with Claude Code conventions"
# GitHub にリポジトリを作ってから
# git remote add origin git@github.com:takumi2911/BX.git
# git push -u origin main
```

6. Claude Code 起動:

```powershell
claude
```

「CLAUDE.md と docs/SPEC.md の内容から、Phase 0 (§23-3) で最初にやるべきことを3つ挙げて」

---

# トラブルシュート早見表

| 症状 | 対処 |
|---|---|
| `claude` コマンドが見つからない | `npm config get prefix` のパスが PATH にあるか確認、なければ追加 |
| Claude Code で `/status` に "API credits" しか出ない | `claude logout` → 環境変数チェック(Phase C-2)→ `claude login` をやり直す |
| UE5 起動時に VS2022 が見つからない | Launcher の**編集 → エディタの環境設定 → ソースコード**で VS2022 指定 |
| C++ 追加後にビルドエラー `missing include` 等 | `.uproject` 右クリック → **Generate Visual Studio project files** を再実行 |
| `.uasset` が LFS に載っていない | `git lfs ls-files` で確認。なければ `git lfs migrate import --include="*.uasset,*.umap"` |
| シェーダーコンパイルが永遠に終わらない | タスクマネージャで CPU 使用率確認。低ければ UE5 再起動。1〜2時間かかるのは正常範囲 |
| 「Plugin failed to load」エラー | `Binaries/`, `Intermediate/` を削除 → Generate Visual Studio project files → 再ビルド |
| プロジェクト開けない(読み込み中フリーズ) | `%LOCALAPPDATA%\UnrealEngine\Common\DerivedDataCache` を削除して再起動 |
| Claude Code 動作が遅い | `/clear` で会話履歴クリア、もしくは一度終了して再起動 |

---

# チェックリスト(このページを閉じる前の確認)

- [ ] Git, Git LFS, Node.js 18+, VS2022 (C++ game dev), UE 5.4, Claude Code が全部動く
- [ ] `ANTHROPIC_API_KEY` 環境変数が **3スコープ全部空**
- [ ] Claude Code の `/status` で **Plan: Pro** と表示される
- [ ] claude.ai の Settings > Usage で **Extra Usage: OFF**
- [ ] Windows 長いパスが有効化されている
- [ ] `D:\Dev\BX\` 配下に CLAUDE.md と SPEC.md が配置され、git 初期化済み
- [ ] テスト C++ プロジェクトが1つビルド・起動できた

全部チェックが付けば、明日から Claude Code に「§23-3 の Phase 0 から始めてください」と言える状態です。
