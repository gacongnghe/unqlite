#pragma once

#include "../include/JsonSchemaTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Binary serialization functions
JsonSchemaError BinarySerializer_SerializeValue(const JsonSchema* schema, const JsonValue* value, BinaryData* output);
JsonSchemaError BinarySerializer_SerializeObject(const JsonSchema* schema, const JsonValue* object, BinaryData* output);
JsonSchemaError BinarySerializer_SerializeArray(const JsonSchema* schema, const JsonValue* array, BinaryData* output);

// Binary deserialization functions
JsonSchemaError BinarySerializer_DeserializeValue(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output);
JsonSchemaError BinarySerializer_DeserializeObject(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output);
JsonSchemaError BinarySerializer_DeserializeArray(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output);

// Utility functions
void BinarySerializer_WriteUInt32(BinaryData* data, uint32_t value);
void BinarySerializer_WriteUInt8(BinaryData* data, uint8_t value);
void BinarySerializer_WriteString(BinaryData* data, const char* str);
void BinarySerializer_WriteBytes(BinaryData* data, const uint8_t* bytes, size_t count);

uint32_t BinarySerializer_ReadUInt32(const BinaryData* data, size_t* offset);
uint8_t BinarySerializer_ReadUInt8(const BinaryData* data, size_t* offset);
char* BinarySerializer_ReadString(const BinaryData* data, size_t* offset);
void BinarySerializer_ReadBytes(const BinaryData* data, size_t* offset, uint8_t* output, size_t count);

// Memory management
BinaryData* BinaryData_Create(size_t initial_size);
void BinaryData_Free(BinaryData* data);
JsonSchemaError BinaryData_Resize(BinaryData* data, size_t new_size);

#ifdef __cplusplus
}
#endif