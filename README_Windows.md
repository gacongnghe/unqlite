# JsonSchemaSerializer - Windows Build Guide

This guide explains how to build and test the JsonSchemaSerializer library on Windows.

## Prerequisites

- Visual Studio 2019 or later (with C++ development tools)
- Windows 10 or later
- PowerShell 5.0 or later (for PowerShell scripts)

## Quick Start

### Option 1: Batch Scripts (Recommended)

1. Open **Developer Command Prompt for VS** or **Developer PowerShell for VS**
2. Navigate to the project directory
3. Run the build and test script:
   ```cmd
   build_and_test_windows.bat
   ```

### Option 2: Individual Scripts

1. **Build only:**
   ```cmd
   build_windows.bat
   ```

2. **Test only (after building):**
   ```cmd
   test_windows.bat
   ```

### Option 3: PowerShell Scripts

1. **Build only:**
   ```powershell
   .\build_windows.ps1
   ```

2. **Test only (after building):**
   ```powershell
   .\test_windows.ps1
   ```

## What Gets Built

The build process creates:

- **JsonSchemaSerializer.lib** - Static library
- **JsonSchemaSerializerTests.exe** - Unit test executable
- **JsonSchemaSerializerExample.exe** - Example usage executable
- **address.schema.json** and **person.schema.json** - Schema files (copied to build directory)

## Project Structure

```
JsonSchemaSerializer/
├── JsonSchemaSerializer.sln          # Visual Studio solution
├── JsonSchemaSerializer/             # Static library project
│   ├── JsonSchemaSerializer.vcxproj  # Project file
│   ├── include/                      # Public headers
│   │   ├── JsonSchemaTypes.h
│   │   └── JsonSchemaSerializer.h
│   └── src/                          # Source files
│       ├── MinimalJsonParser.cpp
│       ├── SimpleBinarySerializer.cpp
│       ├── SimpleJsonSchemaValidator.cpp
│       └── JsonSchemaSerializer.cpp
├── JsonSchemaSerializerTests/        # Test project
│   ├── JsonSchemaSerializerTests.vcxproj
│   └── [test files]
├── build_windows.bat                 # Windows build script
├── test_windows.bat                  # Windows test script
├── build_and_test_windows.bat        # Combined script
├── build_windows.ps1                 # PowerShell build script
├── test_windows.ps1                  # PowerShell test script
└── CMakeLists.txt                    # CMake configuration
```

## Manual Build (Visual Studio)

1. Open `JsonSchemaSerializer.sln` in Visual Studio
2. Select **Release** or **Debug** configuration
3. Right-click on **JsonSchemaSerializer** project → **Build**
4. Right-click on **JsonSchemaSerializerTests** project → **Build**
5. Right-click on **JsonSchemaSerializerExample** project → **Build**

## Manual Build (Command Line)

```cmd
# Navigate to project directory
cd JsonSchemaSerializer

# Build the solution
msbuild JsonSchemaSerializer.sln /p:Configuration=Release /p:Platform=x64

# Or build individual projects
msbuild JsonSchemaSerializer\JsonSchemaSerializer.vcxproj /p:Configuration=Release
msbuild JsonSchemaSerializerTests\JsonSchemaSerializerTests.vcxproj /p:Configuration=Release
```

## Testing

After building, run the tests:

```cmd
# From the build directory
cd build
JsonSchemaSerializerTests.exe

# Or run the example
JsonSchemaSerializerExample.exe
```

## Troubleshooting

### Common Issues

1. **"Visual Studio compiler not found"**
   - Make sure you're running from Developer Command Prompt or Developer PowerShell
   - Or manually set up the environment variables

2. **"Test executable not found"**
   - Run `build_windows.bat` first
   - Check that the build completed successfully

3. **"Schema files not found"**
   - The build script should copy schema files automatically
   - Manually copy `*.schema.json` files to the build directory if needed

4. **Linker errors**
   - Make sure all source files are included in the project
   - Check that the library was built successfully

### Build Output

The build process will show:
- Compilation progress for each source file
- Any warnings or errors
- Final status (success/failure)

### Test Output

The test process will show:
- Individual test results (PASS/FAIL)
- Overall test summary
- Any error messages

## Features

The JsonSchemaSerializer library provides:

- **JSON Schema Parsing** - Parse JSON schemas from files or strings
- **Binary Serialization** - Convert JSON data to binary format
- **Binary Deserialization** - Convert binary data back to JSON
- **Schema Validation** - Validate JSON data against schemas
- **Memory Management** - Automatic cleanup of allocated resources

## API Usage

```c
#include "JsonSchemaSerializer.h"

// Initialize the library
JsonSchemaSerializer_Initialize();

// Load a schema
JsonSchema* schema = NULL;
JsonSchema_LoadFromFile("address.schema.json", &schema);

// Create JSON data
JsonValue* data = JsonValue_Create(JSON_TYPE_OBJECT);
JsonValue_SetString(JsonValue_Create(JSON_TYPE_STRING), "123 Main St");
JsonValue_SetObjectProperty(data, "street", street_value);

// Serialize to binary
SerializationResult* result = JsonSchema_Serialize(schema, data);
if (result->error == JSONSCHEMA_SUCCESS) {
    // Use result->data.data and result->data.size
}

// Cleanup
SerializationResult_Free(result);
JsonValue_Free(data);
JsonSchema_Free(schema);
JsonSchemaSerializer_Cleanup();
```

## Support

For issues or questions:
1. Check the build output for errors
2. Verify all prerequisites are installed
3. Try building from Visual Studio IDE
4. Check that all files are present in the project