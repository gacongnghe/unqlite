@echo off
echo JsonSchemaSerializer Windows Build and Test Script
echo ==================================================

REM Run build script
call build_windows.bat
if %ERRORLEVEL% neq 0 (
    echo Build failed. Stopping.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed. Now running tests...
echo ========================================
echo.

REM Run test script
call test_windows.bat
set FINAL_RESULT=%ERRORLEVEL%

echo.
echo ========================================
echo Final Result: 
if %FINAL_RESULT% equ 0 (
    echo SUCCESS - All builds and tests completed successfully!
) else (
    echo FAILURE - Some builds or tests failed.
)
echo ========================================

pause