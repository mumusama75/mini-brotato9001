[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [int]$PlayDurationSec = 18,
    [int]$ResolutionX = 1280,
    [int]$ResolutionY = 720,
    [string]$ArtifactDir = 'C:\Users\木\.gemini\antigravity\brain\48737092-1368-42d0-b886-b768c9c45adb'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ProjectRoot = [IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$ProjectFile = Join-Path $ProjectRoot 'Brotato3D.uproject'
$EditorExe = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$QADir = Join-Path $ProjectRoot 'Saved\QA'
New-Item -ItemType Directory -Path $QADir -Force | Out-Null

if (-not (Test-Path -LiteralPath $EditorExe -PathType Leaf)) {
    throw "UnrealEditor.exe not found at $EditorExe"
}

# Clear previous log and screenshots
$LogFile = Join-Path $ProjectRoot 'Saved\Logs\Brotato3D.log'
if (Test-Path -LiteralPath $LogFile) {
    Remove-Item -LiteralPath $LogFile -Force -ErrorAction SilentlyContinue
}
Get-ChildItem -Path $QADir -Filter "*.png" -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " Launching Mini Brotato 3D Play QA Slice" -ForegroundColor Cyan
Write-Host " Resolution: ${ResolutionX}x${ResolutionY}, Duration: ${PlayDurationSec}s" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

$GameArgs = @(
    "`"$ProjectFile`"",
    "/Game/Maps/L_BrotatoArena",
    "-game",
    "-windowed",
    "-ResX=$ResolutionX",
    "-ResY=$ResolutionY",
    "-WinX=80",
    "-WinY=80",
    "-NoVSync",
    "-log",
    "-unattended",
    "-B3DQA"
)

$ProcessInfo = New-Object System.Diagnostics.ProcessStartInfo
$ProcessInfo.FileName = $EditorExe
$ProcessInfo.Arguments = ($GameArgs -join ' ')
$ProcessInfo.WorkingDirectory = $ProjectRoot
$ProcessInfo.UseShellExecute = $false

$Proc = [System.Diagnostics.Process]::Start($ProcessInfo)
Write-Host "Process started with PID: $($Proc.Id)" -ForegroundColor Green

$Elapsed = 0
while (-not $Proc.HasExited -and $Elapsed -lt ($PlayDurationSec + 8)) {
    Start-Sleep -Seconds 2
    $Elapsed += 2
    $Proc.Refresh()
    Write-Host "Running QA slice... (${Elapsed}s elapsed)" -ForegroundColor DarkGray
}

if (-not $Proc.HasExited) {
    Write-Host "Stopping QA test run after timeout..." -ForegroundColor Yellow
    $Proc.CloseMainWindow() | Out-Null
    Start-Sleep -Seconds 2
    if (-not $Proc.HasExited) {
        $Proc.Kill()
    }
}

Write-Host "Game session completed." -ForegroundColor Cyan

# Gather captured screenshots from Saved/QA
$CapturedScreenshots = @(Get-ChildItem -Path $QADir -Filter "*.png" -ErrorAction SilentlyContinue)
Write-Host "Found $($CapturedScreenshots.Count) screenshot(s) in $QADir." -ForegroundColor Cyan
foreach ($Shot in $CapturedScreenshots) {
    Write-Host "Captured: $($Shot.Name) ($($Shot.Length) bytes)" -ForegroundColor Green
    if (Test-Path -LiteralPath $ArtifactDir) {
        $Dest = Join-Path $ArtifactDir $Shot.Name
        Copy-Item -LiteralPath $Shot.FullName -Destination $Dest -Force
        Write-Host "Copied to Artifacts: $Dest" -ForegroundColor Green
    }
}

# Also check Saved/Screenshots
$EngineScreenDir = Join-Path $ProjectRoot 'Saved\Screenshots'
if (Test-Path -LiteralPath $EngineScreenDir) {
    Get-ChildItem -Path $EngineScreenDir -Filter "*.png" -Recurse | ForEach-Object {
        Write-Host "Captured Engine Screenshot: $($_.Name) ($($_.Length) bytes)" -ForegroundColor Green
        if (Test-Path -LiteralPath $ArtifactDir) {
            $Dest = Join-Path $ArtifactDir $_.Name
            Copy-Item -LiteralPath $_.FullName -Destination $Dest -Force
        }
    }
}

# Inspect logs
if (Test-Path -LiteralPath $LogFile) {
    $LogContent = Get-Content -LiteralPath $LogFile
    $QALines = $LogContent | Where-Object { $_ -match "B3D_QA:" }
    Write-Host "=== Brotato3D QA Telemetry ===" -ForegroundColor Magenta
    foreach ($Line in $QALines) {
        Write-Host "  $Line" -ForegroundColor Magenta
    }

    $ErrorLines = $LogContent | Where-Object { $_ -match "Fatal error" -or $_ -match "Assertion failed" }
    if ($ErrorLines) {
        Write-Host "Log Errors Found:" -ForegroundColor Red
        $ErrorLines | ForEach-Object { Write-Host "  $_" -ForegroundColor Red }
        throw "Game encountered fatal error during QA run"
    }
    Write-Host "Log verification passed without fatal errors." -ForegroundColor Green
}

Write-Host "Play QA Smoke Test Finished Successfully!" -ForegroundColor Green
