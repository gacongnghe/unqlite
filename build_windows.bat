@echo off
echo JsonSchemaSerializer Windows Build Script
echo ========================================

REM Check if Visual Studio is available
where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Visual Studio compiler not found. Please run from Visual Studio Developer Command Prompt.
    echo Or set up the environment variables manually.
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Build the static library
echo Building JsonSchemaSerializer static library...
cl /c /I..\JsonSchemaSerializer\include /I..\JsonSchemaSerializer\src /DWIN32 /D_WINDOWS /DJSONSCHEMASERIALIZER_EXPORTS /W3 /nologo /O2 /DNDEBUG ..\JsonSchemaSerializer\src\SimpleJsonParser.cpp ..\JsonSchemaSerializer\src\BinarySerializer.cpp ..\JsonSchemaSerializer\src\JsonSchemaValidator.cpp ..\JsonSchemaSerializer\src\JsonSchemaSerializer.cpp
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

REM Create library
lib /OUT:JsonSchemaSerializer.lib *.obj
if %ERRORLEVEL% neq 0 (
    echo Library creation failed!
    pause
    exit /b 1
)

REM Build test executable
echo Building test executable...
cl /I..\JsonSchemaSerializer\include /I..\JsonSchemaSerializer\src /DWIN32 /D_WINDOWS /D_CONSOLE /W3 /nologo /O2 /DNDEBUG ..\JsonSchemaSerializerTests\TestRunner.cpp ..\JsonSchemaSerializerTests\TestJsonSchemaParser.cpp ..\JsonSchemaSerializerTests\TestBinarySerializer.cpp ..\JsonSchemaSerializerTests\TestJsonSchemaValidator.cpp ..\JsonSchemaSerializerTests\TestJsonSchemaSerializer.cpp JsonSchemaSerializer.lib /Fe:JsonSchemaSerializerTests.exe
if %ERRORLEVEL% neq 0 (
    echo Test build failed!
    pause
    exit /b 1
)

REM Build example executable
echo Building example executable...
cl /I..\JsonSchemaSerializer\include /I..\JsonSchemaSerializer\src /DWIN32 /D_WINDOWS /D_CONSOLE /W3 /nologo /O2 /DNDEBUG ..\example_usage.cpp JsonSchemaSerializer.lib /Fe:JsonSchemaSerializerExample.exe
if %ERRORLEVEL% neq 0 (
    echo Example build failed!
    pause
    exit /b 1
)

REM Copy schema files
copy ..\*.schema.json . >nul 2>&1

echo Build completed successfully!
echo.
echo Files created:
echo - JsonSchemaSerializer.lib (static library)
echo - JsonSchemaSerializerTests.exe (test executable)
echo - JsonSchemaSerializerExample.exe (example executable)
echo.
pause