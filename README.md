# JsonSchemaSerializer

A C/C++ static library for JSON schema-based binary serialization and deserialization. This library allows you to define data structures using JSON schemas and serialize/deserialize them to/from compact binary format.

## Features

- **JSON Schema Support**: Load and parse JSON schemas with support for objects, arrays, and primitive types
- **Binary Serialization**: Convert JSON data to compact binary format based on schema definitions
- **Binary Deserialization**: Reconstruct JSON data from binary format using schemas
- **Schema Validation**: Validate data against JSON schemas before serialization
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **C API**: Clean C API for easy integration with other languages
- **Unit Tests**: Comprehensive test suite for all functionality

## Project Structure

```
JsonSchemaSerializer/
├── include/                    # Public header files
│   ├── JsonSchemaTypes.h      # Core data types and structures
│   └── JsonSchemaSerializer.h # Main public API
├── src/                       # Implementation files
│   ├── JsonSchemaParser.h     # JSON schema parsing
│   ├── JsonSchemaParser.cpp
│   ├── BinarySerializer.h     # Binary serialization
│   ├── BinarySerializer.cpp
│   ├── JsonSchemaValidator.h  # Schema validation
│   ├── JsonSchemaValidator.cpp
│   └── JsonSchemaSerializer.cpp # Main API implementation
└── JsonSchemaSerializer.vcxproj # Visual Studio project file

JsonSchemaSerializerTests/
├── TestRunner.h               # Test framework
├── TestRunner.cpp
├── TestJsonSchemaParser.h     # Parser tests
├── TestJsonSchemaParser.cpp
├── TestBinarySerializer.h     # Serializer tests
├── TestBinarySerializer.cpp
├── TestJsonSchemaValidator.h  # Validator tests
├── TestJsonSchemaValidator.cpp
└── JsonSchemaSerializerTests.vcxproj # Visual Studio test project

address.schema.json            # Example address schema
person.schema.json             # Example person schema
example_usage.cpp              # Example usage program
CMakeLists.txt                 # CMake build configuration
JsonSchemaSerializer.sln       # Visual Studio solution
README.md                      # This file
```

## Building

### Visual Studio (Windows)

1. Open `JsonSchemaSerializer.sln` in Visual Studio
2. Build the solution (Ctrl+Shift+B)
3. Run tests by building and running `JsonSchemaSerializerTests` project
4. Run example by building and running `JsonSchemaSerializerExample` project

### CMake (Cross-Platform)

```bash
mkdir build
cd build
cmake ..
make
```

To run tests:
```bash
make test
# or
./JsonSchemaSerializerTests
```

To run example:
```bash
./JsonSchemaSerializerExample
```

## Usage

### Basic Example

```c
#include "JsonSchemaSerializer.h"

int main() {
    // Initialize library
    JsonSchemaSerializer_Initialize();
    
    // Load schema
    JsonSchema* schema = NULL;
    JsonSchema_LoadFromFile("address.schema.json", &schema);
    
    // Create data
    JsonValue* address = JsonValue_Create(JSON_TYPE_OBJECT);
    JsonValue* street = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(street, "123 Main St");
    JsonValue_SetObjectProperty(address, "street", street);
    
    // Validate
    JsonSchema_Validate(schema, address);
    
    // Serialize
    SerializationResult* result = JsonSchema_Serialize(schema, address);
    
    // Deserialize
    JsonValue* deserialized = JsonSchema_Deserialize(schema, &result->data, &error);
    
    // Cleanup
    JsonValue_Free(address);
    JsonValue_Free(deserialized);
    JsonSchema_Free(schema);
    SerializationResult_Free(result);
    JsonSchemaSerializer_Cleanup();
    
    return 0;
}
```

### Schema Example

```json
{
  "$id": "https://example.com/schemas/address.json",
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Address",
  "keyId": 11,
  "type": "object",
  "properties": {
    "street": { "type": "string" },
    "city":   { "type": "string" },
    "country":{ "type": "string" }
  },
  "required": ["street","city"]
}
```

## API Reference

### Core Functions

- `JsonSchemaSerializer_Initialize()` - Initialize the library
- `JsonSchemaSerializer_Cleanup()` - Cleanup library resources
- `JsonSchema_LoadFromFile()` - Load schema from file
- `JsonSchema_LoadFromString()` - Load schema from string
- `JsonSchema_Serialize()` - Serialize data to binary
- `JsonSchema_Deserialize()` - Deserialize data from binary
- `JsonSchema_Validate()` - Validate data against schema

### Data Types

- `JsonSchema` - Schema structure
- `JsonValue` - JSON value structure
- `BinaryData` - Binary data container
- `SerializationResult` - Serialization result with error info

### Error Handling

All functions return `JsonSchemaError` codes:
- `JSONSCHEMA_SUCCESS` - Operation successful
- `JSONSCHEMA_ERROR_INVALID_PARAM` - Invalid parameter
- `JSONSCHEMA_ERROR_MEMORY_ALLOCATION` - Memory allocation failed
- `JSONSCHEMA_ERROR_INVALID_JSON` - Invalid JSON format
- `JSONSCHEMA_ERROR_SCHEMA_VALIDATION` - Schema validation failed
- `JSONSCHEMA_ERROR_SERIALIZATION` - Serialization failed
- `JSONSCHEMA_ERROR_DESERIALIZATION` - Deserialization failed
- `JSONSCHEMA_ERROR_FILE_NOT_FOUND` - File not found
- `JSONSCHEMA_ERROR_INVALID_SCHEMA` - Invalid schema

## Binary Format

The binary format is designed for compactness and includes:

1. **Schema Key ID** (4 bytes) - Identifies the schema used
2. **Value Type** (1 byte) - Type of the root value
3. **Data** - Serialized data based on schema

### Object Serialization
- Number of properties (4 bytes)
- For each property:
  - Key length + key string
  - Value type + value data

### Array Serialization
- Number of items (4 bytes)
- For each item: value type + value data

### Primitive Types
- String: length + string data
- Integer: 4-byte signed integer
- Number: 8-byte double
- Boolean: 1-byte (0/1)
- Null: no additional data

## License

This project is provided as-is for educational and development purposes.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## Testing

The project includes comprehensive unit tests covering:
- JSON schema parsing
- Binary serialization/deserialization
- Schema validation
- Error handling
- Memory management

Run tests using the test executable or through CMake's test framework.