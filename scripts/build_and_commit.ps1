# scripts/build_and_commit.ps1
# C++ 変更後のビルド → 成功したら commit までを半自動化するスクリプト。
# 使い方: PowerShell で `.\scripts\build_and_commit.ps1 -Message "feat: §X-Y を実装"`

param(
    [Parameter(Mandatory=$false)]
    [string]$Message = "",

    [Parameter(Mandatory=$false)]
    [string]$ProjectName = "BX",

    [Parameter(Mandatory=$false)]
    [string]$UE5Path = "C:\Program Files\Epic Games\UE_5.4",

    [Parameter(Mandatory=$false)]
    [switch]$SkipBuild,

    [Parameter(Mandatory=$false)]
    [switch]$SkipCommit,

    [Parameter(Mandatory=$false)]
    [switch]$AutoPush
)

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
$uproject = Join-Path $projectRoot "$ProjectName.uproject"

Write-Host ""
Write-Host "=== BLACKOUT EXFIL Build & Commit ===" -ForegroundColor Cyan
Write-Host "Project Root: $projectRoot" -ForegroundColor DarkGray
Write-Host ""

# ---- 0. 前提チェック ----
if (-not (Test-Path $uproject)) {
    Write-Host "❌ .uproject が見つかりません: $uproject" -ForegroundColor Red
    exit 1
}
if (-not (Test-Path $UE5Path)) {
    Write-Host "❌ UE5 が見つかりません: $UE5Path" -ForegroundColor Red
    exit 1
}

# ---- 1. VS プロジェクトファイル再生成 ----
if (-not $SkipBuild) {
    Write-Host "[1/4] Generating Visual Studio project files..." -ForegroundColor Yellow
    $ubt = Join-Path $UE5Path "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    & $ubt -projectfiles -project="$uproject" -game -rocket -progress | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ プロジェクトファイル生成に失敗" -ForegroundColor Red
        exit 1
    }
    Write-Host "    ✓ Done" -ForegroundColor Green
}

# ---- 2. ビルド実行 ----
if (-not $SkipBuild) {
    Write-Host "[2/4] Building Development Editor..." -ForegroundColor Yellow

    # vswhere で最新 VS2022 の MSBuild を探す
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $msbuildPath = & $vsWhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1

    if (-not $msbuildPath) {
        Write-Host "❌ MSBuild が見つかりません。VS2022 のインストールを確認してください" -ForegroundColor Red
        exit 1
    }

    $sln = Join-Path $projectRoot "$ProjectName.sln"
    & $msbuildPath $sln `
        /target:$ProjectName `
        /p:Configuration="Development Editor" `
        /p:Platform=Win64 `
        /verbosity:minimal `
        /m

    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ ビルド失敗" -ForegroundColor Red
        Write-Host "   エラー内容を確認して Claude Code で修正を依頼してください" -ForegroundColor Yellow
        exit 1
    }
    Write-Host "    ✓ Build succeeded" -ForegroundColor Green
}

# ---- 3. Git 状況確認 ----
Write-Host "[3/4] Checking git status..." -ForegroundColor Yellow
Push-Location $projectRoot
try {
    $gitStatus = git status --porcelain
    if ([string]::IsNullOrWhiteSpace($gitStatus)) {
        Write-Host "    (変更なし。commit スキップ)" -ForegroundColor DarkGray
        Pop-Location
        exit 0
    }

    Write-Host "    変更されたファイル:" -ForegroundColor DarkGray
    git status --short | ForEach-Object { Write-Host "    $_" -ForegroundColor DarkGray }
    Write-Host ""
}
finally {
    # Pop-Location は commit 段階でも必要なので残す
}

# ---- 4. コミット ----
if (-not $SkipCommit) {
    if ([string]::IsNullOrWhiteSpace($Message)) {
        Write-Host "[4/4] コミットメッセージが指定されていません" -ForegroundColor Yellow
        $Message = Read-Host "コミットメッセージを入力"
        if ([string]::IsNullOrWhiteSpace($Message)) {
            Write-Host "   (メッセージなし。commit スキップ)" -ForegroundColor DarkGray
            Pop-Location
            exit 0
        }
    }

    Write-Host "[4/4] Committing..." -ForegroundColor Yellow
    git add -A
    git commit -m $Message
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ commit 失敗" -ForegroundColor Red
        Pop-Location
        exit 1
    }
    Write-Host "    ✓ Commit done: $Message" -ForegroundColor Green

    if ($AutoPush) {
        Write-Host "    Pushing to remote..." -ForegroundColor Yellow
        git push
        if ($LASTEXITCODE -eq 0) {
            Write-Host "    ✓ Push done" -ForegroundColor Green
        }
    }
}

Pop-Location

Write-Host ""
Write-Host "=== 完了 ===" -ForegroundColor Cyan
