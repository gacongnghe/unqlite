#pragma once

#include "JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Library initialization and cleanup
JsonSchemaError JsonSchemaSerializer_Initialize(void);
void JsonSchemaSerializer_Cleanup(void);

// Schema management
JsonSchemaError JsonSchema_LoadFromFile(const char* file_path, JsonSchema** schema);
JsonSchemaError JsonSchema_LoadFromString(const char* json_string, JsonSchema** schema);
void JsonSchema_Free(JsonSchema* schema);

// JSON value management
JsonValue* JsonValue_Create(JsonValueType type);
void JsonValue_Free(JsonValue* value);
JsonValue* JsonValue_Copy(const JsonValue* value);

// JSON value setters
JsonSchemaError JsonValue_SetString(JsonValue* value, const char* str);
JsonSchemaError JsonValue_SetInteger(JsonValue* value, int32_t int_val);
JsonSchemaError JsonValue_SetNumber(JsonValue* value, double number_val);
JsonSchemaError JsonValue_SetBoolean(JsonValue* value, int32_t bool_val);
JsonSchemaError JsonValue_SetNull(JsonValue* value);

// Object manipulation
JsonSchemaError JsonValue_SetObjectProperty(JsonValue* object, const char* key, JsonValue* value);
JsonValue* JsonValue_GetObjectProperty(const JsonValue* object, const char* key);
JsonSchemaError JsonValue_AddArrayItem(JsonValue* array, JsonValue* item);

// Serialization/Deserialization
SerializationResult* JsonSchema_Serialize(const JsonSchema* schema, const JsonValue* data);
JsonValue* JsonSchema_Deserialize(const JsonSchema* schema, const BinaryData* binary_data, JsonSchemaError* error);

// Validation
JsonSchemaError JsonSchema_Validate(const JsonSchema* schema, const JsonValue* data);

// Utility functions
const char* JsonSchema_GetErrorString(JsonSchemaError error);
void SerializationResult_Free(SerializationResult* result);

#ifdef __cplusplus
}
#endif