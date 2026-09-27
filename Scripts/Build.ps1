[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ProjectRoot = [IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$ProjectFile = Join-Path $ProjectRoot 'Brotato3D.uproject'
$BuildBatch = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'

if (-not (Test-Path -LiteralPath $BuildBatch -PathType Leaf)) {
    throw "Build.bat not found at: $BuildBatch"
}

$LogDir = Join-Path $ProjectRoot 'Saved\Logs'
New-Item -ItemType Directory -Path $LogDir -Force | Out-Null
$BuildLog = Join-Path $LogDir 'Build.log'

Write-Host "Building Brotato3D with $BuildBatch..." -ForegroundColor Cyan
& $BuildBatch 'Brotato3DEditor' 'Win64' 'Development' "-Project=$ProjectFile" '-WaitMutex' '-NoHotReloadFromIDE' 2>&1 | Tee-Object -FilePath $BuildLog

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE. See $BuildLog"
}
Write-Host "Build succeeded!" -ForegroundColor Green
