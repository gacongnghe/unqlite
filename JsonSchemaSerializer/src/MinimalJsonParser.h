#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Minimal JSON parser that just handles basic schema parsing
JsonSchemaError MinimalJsonParser_ParseSchema(const char* json_string, JsonSchema** schema);

#ifdef __cplusplus
}
#endif