#include "BinarySerializer.h"
#include "../include/JsonSchemaSerializer.h"
#include <stdlib.h>
#include <string.h>

BinaryData* BinaryData_Create(size_t initial_size) {
    BinaryData* data = (BinaryData*)malloc(sizeof(BinaryData));
    if (!data) {
        return NULL;
    }
    
    data->data = (uint8_t*)malloc(initial_size);
    if (!data->data) {
        free(data);
        return NULL;
    }
    
    data->size = initial_size;
    return data;
}

void BinaryData_Free(BinaryData* data) {
    if (data) {
        if (data->data) {
            free(data->data);
        }
        free(data);
    }
}

JsonSchemaError BinaryData_Resize(BinaryData* data, size_t new_size) {
    if (!data) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    uint8_t* new_data = (uint8_t*)realloc(data->data, new_size);
    if (!new_data) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    data->data = new_data;
    data->size = new_size;
    return JSONSCHEMA_SUCCESS;
}

void BinarySerializer_WriteUInt32(BinaryData* data, uint32_t value) {
    if (!data || !data->data) return;
    
    // Write in little-endian format
    data->data[data->size++] = (uint8_t)(value & 0xFF);
    data->data[data->size++] = (uint8_t)((value >> 8) & 0xFF);
    data->data[data->size++] = (uint8_t)((value >> 16) & 0xFF);
    data->data[data->size++] = (uint8_t)((value >> 24) & 0xFF);
}

void BinarySerializer_WriteUInt8(BinaryData* data, uint8_t value) {
    if (!data || !data->data) return;
    
    data->data[data->size++] = value;
}

void BinarySerializer_WriteString(BinaryData* data, const char* str) {
    if (!data || !data->data || !str) return;
    
    size_t len = strlen(str);
    BinarySerializer_WriteUInt32(data, (uint32_t)len);
    
    // Ensure we have enough space
    if (data->size + len > data->size) {
        BinaryData_Resize(data, data->size + len);
    }
    
    memcpy(data->data + data->size, str, len);
    data->size += len;
}

void BinarySerializer_WriteBytes(BinaryData* data, const uint8_t* bytes, size_t count) {
    if (!data || !data->data || !bytes) return;
    
    // Ensure we have enough space
    if (data->size + count > data->size) {
        BinaryData_Resize(data, data->size + count);
    }
    
    memcpy(data->data + data->size, bytes, count);
    data->size += count;
}

uint32_t BinarySerializer_ReadUInt32(const BinaryData* data, size_t* offset) {
    if (!data || !data->data || !offset || *offset + 4 > data->size) {
        return 0;
    }
    
    uint32_t value = data->data[*offset] | 
                    (data->data[*offset + 1] << 8) |
                    (data->data[*offset + 2] << 16) |
                    (data->data[*offset + 3] << 24);
    
    *offset += 4;
    return value;
}

uint8_t BinarySerializer_ReadUInt8(const BinaryData* data, size_t* offset) {
    if (!data || !data->data || !offset || *offset >= data->size) {
        return 0;
    }
    
    uint8_t value = data->data[*offset];
    (*offset)++;
    return value;
}

char* BinarySerializer_ReadString(const BinaryData* data, size_t* offset) {
    if (!data || !data->data || !offset) {
        return NULL;
    }
    
    uint32_t len = BinarySerializer_ReadUInt32(data, offset);
    if (*offset + len > data->size) {
        return NULL;
    }
    
    char* str = (char*)malloc(len + 1);
    if (!str) {
        return NULL;
    }
    
    memcpy(str, data->data + *offset, len);
    str[len] = '\0';
    *offset += len;
    
    return str;
}

void BinarySerializer_ReadBytes(const BinaryData* data, size_t* offset, uint8_t* output, size_t count) {
    if (!data || !data->data || !offset || !output || *offset + count > data->size) {
        return;
    }
    
    memcpy(output, data->data + *offset, count);
    *offset += count;
}

JsonSchemaError BinarySerializer_SerializeValue(const JsonSchema* schema, const JsonValue* value, BinaryData* output) {
    if (!schema || !value || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Write value type
    BinarySerializer_WriteUInt8(output, (uint8_t)value->type);
    
    switch (value->type) {
        case JSON_TYPE_NULL:
            // No additional data needed
            break;
            
        case JSON_TYPE_BOOLEAN:
            BinarySerializer_WriteUInt8(output, (uint8_t)value->value.int_value);
            break;
            
        case JSON_TYPE_INTEGER:
            BinarySerializer_WriteUInt32(output, (uint32_t)value->value.int_value);
            break;
            
        case JSON_TYPE_NUMBER:
            // Write as 8 bytes (double)
            BinarySerializer_WriteBytes(output, (const uint8_t*)&value->value.number_value, sizeof(double));
            break;
            
        case JSON_TYPE_STRING:
            BinarySerializer_WriteString(output, value->value.string_value);
            break;
            
        case JSON_TYPE_ARRAY:
            return BinarySerializer_SerializeArray(schema, value, output);
            
        case JSON_TYPE_OBJECT:
            return BinarySerializer_SerializeObject(schema, value, output);
            
        default:
            return JSONSCHEMA_ERROR_SERIALIZATION;
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError BinarySerializer_SerializeObject(const JsonSchema* schema, const JsonValue* object, BinaryData* output) {
    if (!schema || !object || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Write number of properties
    BinarySerializer_WriteUInt32(output, (uint32_t)object->value.object_value.count);
    
    // Write each property
    for (size_t i = 0; i < object->value.object_value.count; i++) {
        // Write property key
        BinarySerializer_WriteString(output, object->value.object_value.keys[i]);
        
        // Find corresponding schema for this property
        JsonValue* property_schema = NULL;
        for (size_t j = 0; j < schema->properties.count; j++) {
            if (strcmp(schema->properties.keys[j], object->value.object_value.keys[i]) == 0) {
                property_schema = &schema->properties.values[j];
                break;
            }
        }
        
        // Create temporary schema for serialization
        JsonSchema temp_schema = {0};
        if (property_schema) {
            temp_schema.type = property_schema->type;
        }
        
        // Serialize property value
        JsonSchemaError error = BinarySerializer_SerializeValue(&temp_schema, &object->value.object_value.values[i], output);
        if (error != JSONSCHEMA_SUCCESS) {
            return error;
        }
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError BinarySerializer_SerializeArray(const JsonSchema* schema, const JsonValue* array, BinaryData* output) {
    if (!schema || !array || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Write number of items
    BinarySerializer_WriteUInt32(output, (uint32_t)array->value.array_value.count);
    
    // Write each item
    for (size_t i = 0; i < array->value.array_value.count; i++) {
        JsonSchemaError error = BinarySerializer_SerializeValue(schema, &array->value.array_value.items[i], output);
        if (error != JSONSCHEMA_SUCCESS) {
            return error;
        }
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError BinarySerializer_DeserializeValue(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output) {
    if (!schema || !data || !offset || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    *output = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!*output) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    // Read value type
    (*output)->type = (JsonValueType)BinarySerializer_ReadUInt8(data, offset);
    
    switch ((*output)->type) {
        case JSON_TYPE_NULL:
            // No additional data needed
            break;
            
        case JSON_TYPE_BOOLEAN:
            (*output)->value.int_value = BinarySerializer_ReadUInt8(data, offset);
            break;
            
        case JSON_TYPE_INTEGER:
            (*output)->value.int_value = (int32_t)BinarySerializer_ReadUInt32(data, offset);
            break;
            
        case JSON_TYPE_NUMBER:
            // Read as 8 bytes (double)
            BinarySerializer_ReadBytes(data, offset, (uint8_t*)&(*output)->value.number_value, sizeof(double));
            break;
            
        case JSON_TYPE_STRING:
            (*output)->value.string_value = BinarySerializer_ReadString(data, offset);
            if (!(*output)->value.string_value) {
                JsonValue_Free(*output);
                *output = NULL;
                return JSONSCHEMA_ERROR_DESERIALIZATION;
            }
            break;
            
        case JSON_TYPE_ARRAY:
            return BinarySerializer_DeserializeArray(schema, data, offset, output);
            
        case JSON_TYPE_OBJECT:
            return BinarySerializer_DeserializeObject(schema, data, offset, output);
            
        default:
            JsonValue_Free(*output);
            *output = NULL;
            return JSONSCHEMA_ERROR_DESERIALIZATION;
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError BinarySerializer_DeserializeObject(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output) {
    if (!schema || !data || !offset || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Read number of properties
    uint32_t property_count = BinarySerializer_ReadUInt32(data, offset);
    
    (*output)->type = JSON_TYPE_OBJECT;
    (*output)->value.object_value.count = property_count;
    (*output)->value.object_value.keys = (char**)malloc(property_count * sizeof(char*));
    (*output)->value.object_value.values = (JsonValue*)malloc(property_count * sizeof(JsonValue));
    
    if (!(*output)->value.object_value.keys || !(*output)->value.object_value.values) {
        JsonValue_Free(*output);
        *output = NULL;
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    // Read each property
    for (uint32_t i = 0; i < property_count; i++) {
        // Read property key
        (*output)->value.object_value.keys[i] = BinarySerializer_ReadString(data, offset);
        if (!(*output)->value.object_value.keys[i]) {
            JsonValue_Free(*output);
            *output = NULL;
            return JSONSCHEMA_ERROR_DESERIALIZATION;
        }
        
        // Find corresponding schema for this property
        JsonValue* property_schema = NULL;
        for (size_t j = 0; j < schema->properties.count; j++) {
            if (strcmp(schema->properties.keys[j], (*output)->value.object_value.keys[i]) == 0) {
                property_schema = &schema->properties.values[j];
                break;
            }
        }
        
        // Create temporary schema for deserialization
        JsonSchema temp_schema = {0};
        if (property_schema) {
            temp_schema.type = property_schema->type;
        }
        
        // Deserialize property value
        JsonValue* value = NULL;
        JsonSchemaError error = BinarySerializer_DeserializeValue(&temp_schema, data, offset, &value);
        if (error != JSONSCHEMA_SUCCESS) {
            JsonValue_Free(*output);
            *output = NULL;
            return error;
        }
        
        (*output)->value.object_value.values[i] = *value;
        free(value);
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError BinarySerializer_DeserializeArray(const JsonSchema* schema, const BinaryData* data, size_t* offset, JsonValue** output) {
    if (!schema || !data || !offset || !output) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Read number of items
    uint32_t item_count = BinarySerializer_ReadUInt32(data, offset);
    
    (*output)->type = JSON_TYPE_ARRAY;
    (*output)->value.array_value.count = item_count;
    (*output)->value.array_value.items = (JsonValue*)malloc(item_count * sizeof(JsonValue));
    
    if (!(*output)->value.array_value.items) {
        JsonValue_Free(*output);
        *output = NULL;
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    // Read each item
    for (uint32_t i = 0; i < item_count; i++) {
        JsonValue* item = NULL;
        JsonSchemaError error = BinarySerializer_DeserializeValue(schema, data, offset, &item);
        if (error != JSONSCHEMA_SUCCESS) {
            JsonValue_Free(*output);
            *output = NULL;
            return error;
        }
        
        (*output)->value.array_value.items[i] = *item;
        free(item);
    }
    
    return JSONSCHEMA_SUCCESS;
}