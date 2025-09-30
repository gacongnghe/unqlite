@echo off
echo JsonSchemaSerializer Windows Test Script
echo ========================================

REM Check if we're in the build directory
if not exist JsonSchemaSerializerTests.exe (
    echo Test executable not found. Please run build_windows.bat first.
    pause
    exit /b 1
)

REM Check if schema files exist
if not exist address.schema.json (
    echo Schema files not found. Please run build_windows.bat first.
    pause
    exit /b 1
)

echo Running unit tests...
echo.
JsonSchemaSerializerTests.exe
set TEST_RESULT=%ERRORLEVEL%

echo.
echo Running example...
echo.
JsonSchemaSerializerExample.exe
set EXAMPLE_RESULT=%ERRORLEVEL%

echo.
echo ========================================
echo Test Results Summary:
echo ========================================
if %TEST_RESULT% equ 0 (
    echo Unit Tests: PASSED
) else (
    echo Unit Tests: FAILED (Exit code: %TEST_RESULT%)
)

if %EXAMPLE_RESULT% equ 0 (
    echo Example: PASSED
) else (
    echo Example: FAILED (Exit code: %EXAMPLE_RESULT%)
)

if %TEST_RESULT% equ 0 if %EXAMPLE_RESULT% equ 0 (
    echo.
    echo All tests passed successfully!
) else (
    echo.
    echo Some tests failed. Check the output above for details.
)

echo.
pause