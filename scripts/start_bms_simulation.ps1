param(
    [switch]$NoBrowser
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$exePath = Join-Path $root '.pio\build\native\program.exe'
$dashboardPath = Join-Path $root 'web\index.html'

if (-not (Test-Path $exePath)) {
    Write-Host 'Binario do firmware native nao encontrado.' -ForegroundColor Yellow
    Write-Host 'Execute primeiro: pio run -e native' -ForegroundColor Cyan
    exit 1
}

if (-not (Test-Path $dashboardPath)) {
    Write-Host 'Dashboard web nao encontrado em web/index.html.' -ForegroundColor Yellow
    exit 1
}

Write-Host 'Iniciando simulacao do BMS em modo native...' -ForegroundColor Green
$proc = Start-Process -FilePath $exePath -WorkingDirectory $root -PassThru

if (-not $NoBrowser) {
    Write-Host 'Abrindo dashboard web...' -ForegroundColor Green
    Start-Process $dashboardPath
}

Write-Host "BMS iniciado com PID: $($proc.Id)" -ForegroundColor Cyan
Write-Host "Dashboard local: $dashboardPath" -ForegroundColor Cyan
Write-Host 'Aguarde a execucao do firmware. Pressione Ctrl+C no terminal para encerrar o processo do BMS.' -ForegroundColor Yellow

try {
    $proc.WaitForExit()
}
catch {
    Write-Host 'O processo foi encerrado.' -ForegroundColor Yellow
}
