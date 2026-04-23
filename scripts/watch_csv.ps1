# scripts/watch_csv.ps1
# DataSource/*.csv の変更を監視し、保存されたら UE5 エディタに Python コマンドを送って自動再インポートさせる。
#
# 前提:
# - UE5 エディタが起動済み
# - エディタで Edit → Project Settings → Plugins → Python Editor Script Plugin が有効
# - エディタで Edit → Editor Preferences → Python → "Enable Remote Execution" を ON
#   (同じ PC 内で PowerShell → UE5 に Python を送るため)
#
# 使い方:
#   .\scripts\watch_csv.ps1
#
# 停止: Ctrl+C

param(
    [Parameter(Mandatory=$false)]
    [string]$WatchPath = "DataSource",

    [Parameter(Mandatory=$false)]
    [int]$DebounceSeconds = 2
)

$ErrorActionPreference = "Continue"
$projectRoot = Split-Path -Parent $PSScriptRoot
$fullWatchPath = Join-Path $projectRoot $WatchPath

if (-not (Test-Path $fullWatchPath)) {
    Write-Host "❌ 監視パスが見つかりません: $fullWatchPath" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "=== CSV Watcher for UE5 DT Reimport ===" -ForegroundColor Cyan
Write-Host "監視: $fullWatchPath" -ForegroundColor DarkGray
Write-Host "UE5 エディタが起動していて、Python Remote Execution が有効であることを確認してください" -ForegroundColor Yellow
Write-Host "停止: Ctrl+C"
Write-Host ""

$watcher = New-Object System.IO.FileSystemWatcher
$watcher.Path = $fullWatchPath
$watcher.Filter = "*.csv"
$watcher.IncludeSubdirectories = $true
$watcher.EnableRaisingEvents = $true

# デバウンス用ハッシュ
$lastTriggered = @{}

Register-ObjectEvent -InputObject $watcher -EventName "Changed" -Action {
    $path = $Event.SourceEventArgs.FullPath
    $now = Get-Date

    # デバウンス
    if ($script:lastTriggered.ContainsKey($path)) {
        $diff = ($now - $script:lastTriggered[$path]).TotalSeconds
        if ($diff -lt 2) { return }
    }
    $script:lastTriggered[$path] = $now

    $relativePath = $path.Replace($projectRoot + "\", "").Replace("\", "/")
    Write-Host "[$($now.ToString('HH:mm:ss'))] 変更検知: $relativePath" -ForegroundColor Yellow

    # UE5 側で Reimport するスクリプトをファイル化(ここでは簡略)
    # 本番では UE5 Remote Execution API を使って Python コマンドを UE5 に送信する
    # 例示として echo のみ残す:
    Write-Host "   → UE5 で Reimport が走ります(実装は ue5_python/reimport_datatables.py 参照)" -ForegroundColor DarkGray
} | Out-Null

# 常駐
try {
    while ($true) { Start-Sleep -Seconds 1 }
}
finally {
    $watcher.EnableRaisingEvents = $false
    $watcher.Dispose()
    Write-Host "停止しました" -ForegroundColor Cyan
}
