# setup-check.ps1
# 環境チェックスクリプト: 必要なツールが揃っているかを確認する
# 使い方: PowerShell で `.\scripts\setup-check.ps1`

$ErrorActionPreference = "Continue"

function Test-Command {
    param([string]$Name, [string]$Command, [string]$MinVersion = $null)
    Write-Host -NoNewline "[$Name] ... "
    try {
        $output = Invoke-Expression $Command 2>&1
        if ($LASTEXITCODE -eq 0 -or $output) {
            Write-Host "OK" -ForegroundColor Green
            Write-Host "    → $($output -split "`n" | Select-Object -First 1)" -ForegroundColor DarkGray
            return $true
        }
    } catch {}
    Write-Host "NOT FOUND" -ForegroundColor Red
    return $false
}

Write-Host ""
Write-Host "=== UE5 + Claude Code 環境チェック ===" -ForegroundColor Cyan
Write-Host ""

$results = @()
$results += @{ Name = "Git";         OK = (Test-Command "Git"          "git --version") }
$results += @{ Name = "Git LFS";     OK = (Test-Command "Git LFS"      "git lfs --version") }
$results += @{ Name = "Node.js";     OK = (Test-Command "Node.js"      "node --version") }
$results += @{ Name = "npm";         OK = (Test-Command "npm"          "npm --version") }
$results += @{ Name = "Claude Code"; OK = (Test-Command "Claude Code"  "claude --version") }

# UE5 インストール検出(Epic Games Launcher の既定パス)
# 本プロジェクトは 5.4 固定
Write-Host -NoNewline "[Unreal Engine 5.4] ... "
$uePaths = @(
    "C:\Program Files\Epic Games\UE_5.4",
    "D:\Epic Games\UE_5.4",
    "E:\Epic Games\UE_5.4"
)
$ueFound = $uePaths | Where-Object { Test-Path $_ } | Select-Object -First 1
if ($ueFound) {
    Write-Host "OK" -ForegroundColor Green
    Write-Host "    → $ueFound" -ForegroundColor DarkGray
    $results += @{ Name = "UE 5.4"; OK = $true }
} else {
    # 他バージョンが入っていないか念のため確認
    $otherPaths = @(
        "C:\Program Files\Epic Games\UE_5.3",
        "C:\Program Files\Epic Games\UE_5.5",
        "C:\Program Files\Epic Games\UE_5.6"
    )
    $otherFound = $otherPaths | Where-Object { Test-Path $_ } | Select-Object -First 1
    if ($otherFound) {
        Write-Host "WRONG VERSION" -ForegroundColor Yellow
        Write-Host "    → $otherFound が見つかりましたが、本プロジェクトは 5.4 固定です" -ForegroundColor Yellow
    } else {
        Write-Host "NOT FOUND" -ForegroundColor Red
        Write-Host "    → Epic Games Launcher から UE 5.4 をインストールしてください" -ForegroundColor Yellow
    }
    $results += @{ Name = "UE 5.4"; OK = $false }
}

# Visual Studio 2022 検出
Write-Host -NoNewline "[Visual Studio 2022] ... "
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath 2>$null
    if ($vsPath) {
        Write-Host "OK" -ForegroundColor Green
        Write-Host "    → $vsPath" -ForegroundColor DarkGray
        $results += @{ Name = "VS2022"; OK = $true }
    } else {
        Write-Host "INCOMPLETE" -ForegroundColor Yellow
        Write-Host "    → C++ ワークロードがインストールされていません" -ForegroundColor Yellow
        $results += @{ Name = "VS2022"; OK = $false }
    }
} else {
    Write-Host "NOT FOUND" -ForegroundColor Red
    $results += @{ Name = "VS2022"; OK = $false }
}

Write-Host ""
Write-Host "=== サマリ ===" -ForegroundColor Cyan
$missing = $results | Where-Object { -not $_.OK }
if ($missing.Count -eq 0) {
    Write-Host "✓ すべての必要ツールが揃っています。" -ForegroundColor Green
    Write-Host ""
    Write-Host "次のステップ: SETUP_GUIDE.md の §4 (UE5 プロジェクト作成) へ進んでください。"
} else {
    Write-Host "✗ 以下が不足しています:" -ForegroundColor Yellow
    $missing | ForEach-Object { Write-Host "  - $($_.Name)" -ForegroundColor Yellow }
    Write-Host ""
    Write-Host "SETUP_GUIDE.md の §1 〜 §3 を参照してインストールしてください。"
}
Write-Host ""
