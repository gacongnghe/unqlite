#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Schema validation functions
JsonSchemaError JsonSchemaValidator_ValidateValue(const JsonSchema* schema, const JsonValue* value);
JsonSchemaError JsonSchemaValidator_ValidateObject(const JsonSchema* schema, const JsonValue* object);
JsonSchemaError JsonSchemaValidator_ValidateArray(const JsonSchema* schema, const JsonValue* array);
JsonSchemaError JsonSchemaValidator_ValidateString(const JsonSchema* schema, const JsonValue* value);
JsonSchemaError JsonSchemaValidator_ValidateInteger(const JsonSchema* schema, const JsonValue* value);
JsonSchemaError JsonSchemaValidator_ValidateNumber(const JsonSchema* schema, const JsonValue* value);

// Required field validation
JsonSchemaError JsonSchemaValidator_ValidateRequiredFields(const JsonSchema* schema, const JsonValue* object);

// Type validation
int32_t JsonSchemaValidator_IsValidType(JsonValueType schema_type, JsonValueType value_type);

#ifdef __cplusplus
}
#endif