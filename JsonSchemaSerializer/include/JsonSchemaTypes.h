#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct JsonSchema JsonSchema;
typedef struct JsonValue JsonValue;
typedef struct BinaryData BinaryData;
typedef struct SerializationResult SerializationResult;

// Error codes
typedef enum {
    JSONSCHEMA_SUCCESS = 0,
    JSONSCHEMA_ERROR_INVALID_PARAM = -1,
    JSONSCHEMA_ERROR_MEMORY_ALLOCATION = -2,
    JSONSCHEMA_ERROR_INVALID_JSON = -3,
    JSONSCHEMA_ERROR_SCHEMA_VALIDATION = -4,
    JSONSCHEMA_ERROR_SERIALIZATION = -5,
    JSONSCHEMA_ERROR_DESERIALIZATION = -6,
    JSONSCHEMA_ERROR_FILE_NOT_FOUND = -7,
    JSONSCHEMA_ERROR_INVALID_SCHEMA = -8
} JsonSchemaError;

// JSON value types
typedef enum {
    JSON_TYPE_NULL = 0,
    JSON_TYPE_BOOLEAN = 1,
    JSON_TYPE_INTEGER = 2,
    JSON_TYPE_NUMBER = 3,
    JSON_TYPE_STRING = 4,
    JSON_TYPE_ARRAY = 5,
    JSON_TYPE_OBJECT = 6
} JsonValueType;

// Binary data structure
typedef struct BinaryData {
    uint8_t* data;
    size_t size;
} BinaryData;

// Serialization result
typedef struct SerializationResult {
    JsonSchemaError error;
    BinaryData data;
    char* error_message;
} SerializationResult;

// JSON value structure
typedef struct JsonValue {
    JsonValueType type;
    union {
        int32_t int_value;
        double number_value;
        char* string_value;
        struct {
            JsonValue* items;
            size_t count;
        } array_value;
        struct {
            char** keys;
            JsonValue* values;
            size_t count;
        } object_value;
    } value;
} JsonValue;

// JSON schema structure
typedef struct JsonSchema {
    char* id;
    char* title;
    uint32_t key_id;
    JsonValueType type;
    struct {
        char** required_fields;
        size_t required_count;
    } required;
    struct {
        char** keys;
        JsonValue* values;
        size_t count;
    } properties;
    struct {
        JsonValue* items;
        size_t count;
    } array_items;
    char* ref; // For $ref references
} JsonSchema;

#ifdef __cplusplus
}
#endif