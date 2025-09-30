#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Internal JSON parsing functions
JsonSchemaError JsonSchemaParser_ParseSchema(const char* json_string, JsonSchema** schema);
JsonSchemaError JsonSchemaParser_ParseValue(const char* json_string, JsonValue** value);
JsonSchemaError JsonSchemaParser_ParseValueWithOffset(const char* json_string, size_t* offset, JsonValue** value);
JsonSchemaError JsonSchemaParser_ParseObject(const char* json_string, JsonValue** object);
JsonSchemaError JsonSchemaParser_ParseObjectWithOffset(const char* json_string, size_t* offset, JsonValue** object);
JsonSchemaError JsonSchemaParser_ParseArray(const char* json_string, JsonValue** array);
JsonSchemaError JsonSchemaParser_ParseArrayWithOffset(const char* json_string, size_t* offset, JsonValue** array);

// Internal utility functions
char* JsonSchemaParser_ReadString(const char* json_string, size_t* offset);
int32_t JsonSchemaParser_ReadInteger(const char* json_string, size_t* offset);
double JsonSchemaParser_ReadNumber(const char* json_string, size_t* offset);
int32_t JsonSchemaParser_ReadBoolean(const char* json_string, size_t* offset);
void JsonSchemaParser_SkipWhitespace(const char* json_string, size_t* offset);

#ifdef __cplusplus
}
#endif