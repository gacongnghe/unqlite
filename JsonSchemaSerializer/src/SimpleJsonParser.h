#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Simple JSON parser that handles the basic cases we need
JsonSchemaError SimpleJsonParser_ParseSchema(const char* json_string, JsonSchema** schema);
JsonSchemaError SimpleJsonParser_ParseValue(const char* json_string, JsonValue** value);
JsonSchemaError SimpleJsonParser_ParseValueWithOffset(const char* json_string, size_t* offset, JsonValue** value);
JsonSchemaError SimpleJsonParser_ParseObjectWithOffset(const char* json_string, size_t* offset, JsonValue** object);
JsonSchemaError SimpleJsonParser_ParseArrayWithOffset(const char* json_string, size_t* offset, JsonValue** array);

// Helper functions
char* SimpleJsonParser_ReadString(const char* json_string, size_t* offset);
int32_t SimpleJsonParser_ReadInteger(const char* json_string, size_t* offset);
void SimpleJsonParser_SkipWhitespace(const char* json_string, size_t* offset);

#ifdef __cplusplus
}
#endif