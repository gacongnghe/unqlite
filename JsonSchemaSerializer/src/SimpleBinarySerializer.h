#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Simple binary serializer for testing
JsonSchemaError BinarySerializer_Serialize(const JsonValue* value, const JsonSchema* schema, uint8_t** buffer, size_t* buffer_size);
JsonSchemaError BinarySerializer_Deserialize(const uint8_t* buffer, size_t buffer_size, const JsonSchema* schema, JsonValue** value);

#ifdef __cplusplus
}
#endif