#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Simple JSON schema validator for testing
JsonSchemaError JsonSchemaValidator_Validate(const JsonValue* data, const JsonSchema* schema);

#ifdef __cplusplus
}
#endif