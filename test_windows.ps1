# JsonSchemaSerializer Windows PowerShell Test Script
Write-Host "JsonSchemaSerializer Windows Test Script" -ForegroundColor Green
Write-Host "=======================================" -ForegroundColor Green

# Check if we're in the build directory
if (!(Test-Path "JsonSchemaSerializerTests.exe")) {
    Write-Host "Test executable not found. Please run build_windows.ps1 first." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if schema files exist
if (!(Test-Path "address.schema.json")) {
    Write-Host "Schema files not found. Please run build_windows.ps1 first." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Running unit tests..." -ForegroundColor Yellow
Write-Host ""
& .\JsonSchemaSerializerTests.exe
$testResult = $LASTEXITCODE

Write-Host ""
Write-Host "Running example..." -ForegroundColor Yellow
Write-Host ""
& .\JsonSchemaSerializerExample.exe
$exampleResult = $LASTEXITCODE

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Test Results Summary:" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($testResult -eq 0) {
    Write-Host "Unit Tests: PASSED" -ForegroundColor Green
} else {
    Write-Host "Unit Tests: FAILED (Exit code: $testResult)" -ForegroundColor Red
}

if ($exampleResult -eq 0) {
    Write-Host "Example: PASSED" -ForegroundColor Green
} else {
    Write-Host "Example: FAILED (Exit code: $exampleResult)" -ForegroundColor Red
}

if ($testResult -eq 0 -and $exampleResult -eq 0) {
    Write-Host ""
    Write-Host "All tests passed successfully!" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "Some tests failed. Check the output above for details." -ForegroundColor Red
}

Write-Host ""
Read-Host "Press Enter to continue"