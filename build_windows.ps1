# JsonSchemaSerializer Windows PowerShell Build Script
Write-Host "JsonSchemaSerializer Windows Build Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

# Check if Visual Studio is available
try {
    $null = Get-Command cl -ErrorAction Stop
    Write-Host "Visual Studio compiler found." -ForegroundColor Green
} catch {
    Write-Host "Visual Studio compiler not found. Please run from Visual Studio Developer PowerShell." -ForegroundColor Red
    Write-Host "Or set up the environment variables manually." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}
Set-Location "build"

# Build the static library
Write-Host "Building JsonSchemaSerializer static library..." -ForegroundColor Yellow
$compileArgs = @(
    "/c", "/I..\JsonSchemaSerializer\include", "/I..\JsonSchemaSerializer\src",
    "/DWIN32", "/D_WINDOWS", "/DJSONSCHEMASERIALIZER_EXPORTS",
    "/W3", "/nologo", "/O2", "/DNDEBUG",
    "..\JsonSchemaSerializer\src\SimpleJsonParser.cpp",
    "..\JsonSchemaSerializer\src\BinarySerializer.cpp",
    "..\JsonSchemaSerializer\src\JsonSchemaValidator.cpp",
    "..\JsonSchemaSerializer\src\JsonSchemaSerializer.cpp"
)

& cl @compileArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Create library
Write-Host "Creating static library..." -ForegroundColor Yellow
& lib /OUT:JsonSchemaSerializer.lib *.obj
if ($LASTEXITCODE -ne 0) {
    Write-Host "Library creation failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Build test executable
Write-Host "Building test executable..." -ForegroundColor Yellow
$testArgs = @(
    "/I..\JsonSchemaSerializer\include", "/I..\JsonSchemaSerializer\src",
    "/DWIN32", "/D_WINDOWS", "/D_CONSOLE",
    "/W3", "/nologo", "/O2", "/DNDEBUG",
    "..\JsonSchemaSerializerTests\TestRunner.cpp",
    "..\JsonSchemaSerializerTests\TestJsonSchemaParser.cpp",
    "..\JsonSchemaSerializerTests\TestBinarySerializer.cpp",
    "..\JsonSchemaSerializerTests\TestJsonSchemaValidator.cpp",
    "..\JsonSchemaSerializerTests\TestJsonSchemaSerializer.cpp",
    "JsonSchemaSerializer.lib",
    "/Fe:JsonSchemaSerializerTests.exe"
)

& cl @testArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Test build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Build example executable
Write-Host "Building example executable..." -ForegroundColor Yellow
$exampleArgs = @(
    "/I..\JsonSchemaSerializer\include", "/I..\JsonSchemaSerializer\src",
    "/DWIN32", "/D_WINDOWS", "/D_CONSOLE",
    "/W3", "/nologo", "/O2", "/DNDEBUG",
    "..\example_usage.cpp",
    "JsonSchemaSerializer.lib",
    "/Fe:JsonSchemaSerializerExample.exe"
)

& cl @exampleArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Example build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Copy schema files
Write-Host "Copying schema files..." -ForegroundColor Yellow
Copy-Item "..\*.schema.json" "." -ErrorAction SilentlyContinue

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Files created:" -ForegroundColor Cyan
Write-Host "- JsonSchemaSerializer.lib (static library)" -ForegroundColor White
Write-Host "- JsonSchemaSerializerTests.exe (test executable)" -ForegroundColor White
Write-Host "- JsonSchemaSerializerExample.exe (example executable)" -ForegroundColor White
Write-Host ""
Read-Host "Press Enter to continue"