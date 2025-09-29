#include "JsonSchemaSerializer.h"
#include "JsonSchemaParser.h"
#include "BinarySerializer.h"
#include "JsonSchemaValidator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global state for library initialization
static int g_library_initialized = 0;

JsonSchemaError JsonSchemaSerializer_Initialize(void) {
    g_library_initialized = 1;
    return JSONSCHEMA_SUCCESS;
}

void JsonSchemaSerializer_Cleanup(void) {
    g_library_initialized = 0;
}

JsonSchemaError JsonSchema_LoadFromFile(const char* file_path, JsonSchema** schema) {
    if (!file_path || !schema) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return JSONSCHEMA_ERROR_FILE_NOT_FOUND;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file content
    char* json_string = (char*)malloc(file_size + 1);
    if (!json_string) {
        fclose(file);
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    size_t bytes_read = fread(json_string, 1, file_size, file);
    fclose(file);
    
    if (bytes_read != file_size) {
        free(json_string);
        return JSONSCHEMA_ERROR_FILE_NOT_FOUND;
    }
    
    json_string[file_size] = '\0';
    
    // Parse JSON
    JsonSchemaError error = JsonSchema_LoadFromString(json_string, schema);
    free(json_string);
    
    return error;
}

JsonSchemaError JsonSchema_LoadFromString(const char* json_string, JsonSchema** schema) {
    if (!json_string || !schema) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    return JsonSchemaParser_ParseSchema(json_string, schema);
}

void JsonSchema_Free(JsonSchema* schema) {
    if (!schema) {
        return;
    }
    
    if (schema->id) {
        free(schema->id);
    }
    if (schema->title) {
        free(schema->title);
    }
    if (schema->ref) {
        free(schema->ref);
    }
    
    // Free required fields
    for (size_t i = 0; i < schema->required.required_count; i++) {
        free(schema->required.required_fields[i]);
    }
    if (schema->required.required_fields) {
        free(schema->required.required_fields);
    }
    
    // Free properties
    for (size_t i = 0; i < schema->properties.count; i++) {
        free(schema->properties.keys[i]);
        JsonValue_Free(&schema->properties.values[i]);
    }
    if (schema->properties.keys) {
        free(schema->properties.keys);
    }
    if (schema->properties.values) {
        free(schema->properties.values);
    }
    
    // Free array items
    for (size_t i = 0; i < schema->array_items.count; i++) {
        JsonValue_Free(&schema->array_items.items[i]);
    }
    if (schema->array_items.items) {
        free(schema->array_items.items);
    }
    
    free(schema);
}

JsonValue* JsonValue_Create(JsonValueType type) {
    JsonValue* value = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!value) {
        return NULL;
    }
    
    value->type = type;
    return value;
}

void JsonValue_Free(JsonValue* value) {
    if (!value) {
        return;
    }
    
    switch (value->type) {
        case JSON_TYPE_STRING:
            if (value->value.string_value) {
                free(value->value.string_value);
            }
            break;
            
        case JSON_TYPE_ARRAY:
            for (size_t i = 0; i < value->value.array_value.count; i++) {
                JsonValue_Free(&value->value.array_value.items[i]);
            }
            if (value->value.array_value.items) {
                free(value->value.array_value.items);
            }
            break;
            
        case JSON_TYPE_OBJECT:
            for (size_t i = 0; i < value->value.object_value.count; i++) {
                free(value->value.object_value.keys[i]);
                JsonValue_Free(&value->value.object_value.values[i]);
            }
            if (value->value.object_value.keys) {
                free(value->value.object_value.keys);
            }
            if (value->value.object_value.values) {
                free(value->value.object_value.values);
            }
            break;
            
        default:
            break;
    }
}

JsonValue* JsonValue_Copy(const JsonValue* value) {
    if (!value) {
        return NULL;
    }
    
    JsonValue* copy = (JsonValue*)malloc(sizeof(JsonValue));
    if (!copy) {
        return NULL;
    }
    
    copy->type = value->type;
    
    switch (value->type) {
        case JSON_TYPE_NULL:
        case JSON_TYPE_BOOLEAN:
        case JSON_TYPE_INTEGER:
        case JSON_TYPE_NUMBER:
            *copy = *value;
            break;
            
        case JSON_TYPE_STRING:
            if (value->value.string_value) {
                copy->value.string_value = strdup(value->value.string_value);
            } else {
                copy->value.string_value = NULL;
            }
            break;
            
        case JSON_TYPE_ARRAY:
            copy->value.array_value.count = value->value.array_value.count;
            copy->value.array_value.items = (JsonValue*)malloc(value->value.array_value.count * sizeof(JsonValue));
            for (size_t i = 0; i < value->value.array_value.count; i++) {
                copy->value.array_value.items[i] = *JsonValue_Copy(&value->value.array_value.items[i]);
            }
            break;
            
        case JSON_TYPE_OBJECT:
            copy->value.object_value.count = value->value.object_value.count;
            copy->value.object_value.keys = (char**)malloc(value->value.object_value.count * sizeof(char*));
            copy->value.object_value.values = (JsonValue*)malloc(value->value.object_value.count * sizeof(JsonValue));
            for (size_t i = 0; i < value->value.object_value.count; i++) {
                copy->value.object_value.keys[i] = strdup(value->value.object_value.keys[i]);
                copy->value.object_value.values[i] = *JsonValue_Copy(&value->value.object_value.values[i]);
            }
            break;
    }
    
    return copy;
}

JsonSchemaError JsonValue_SetString(JsonValue* value, const char* str) {
    if (!value || value->type != JSON_TYPE_STRING) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (value->value.string_value) {
        free(value->value.string_value);
    }
    
    value->value.string_value = str ? strdup(str) : NULL;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetInteger(JsonValue* value, int32_t int_val) {
    if (!value || value->type != JSON_TYPE_INTEGER) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    value->value.int_value = int_val;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetNumber(JsonValue* value, double number_val) {
    if (!value || value->type != JSON_TYPE_NUMBER) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    value->value.number_value = number_val;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetBoolean(JsonValue* value, int32_t bool_val) {
    if (!value || value->type != JSON_TYPE_BOOLEAN) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    value->value.int_value = bool_val ? 1 : 0;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetNull(JsonValue* value) {
    if (!value || value->type != JSON_TYPE_NULL) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Null values don't need additional data
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetObjectProperty(JsonValue* object, const char* key, JsonValue* value) {
    if (!object || object->type != JSON_TYPE_OBJECT || !key || !value) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Check if property already exists
    for (size_t i = 0; i < object->value.object_value.count; i++) {
        if (strcmp(object->value.object_value.keys[i], key) == 0) {
            // Replace existing property
            JsonValue_Free(&object->value.object_value.values[i]);
            object->value.object_value.values[i] = *value;
            return JSONSCHEMA_SUCCESS;
        }
    }
    
    // Add new property
    size_t new_count = object->value.object_value.count + 1;
    object->value.object_value.keys = (char**)realloc(object->value.object_value.keys, new_count * sizeof(char*));
    object->value.object_value.values = (JsonValue*)realloc(object->value.object_value.values, new_count * sizeof(JsonValue));
    
    if (!object->value.object_value.keys || !object->value.object_value.values) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    object->value.object_value.keys[new_count - 1] = strdup(key);
    object->value.object_value.values[new_count - 1] = *value;
    object->value.object_value.count = new_count;
    
    return JSONSCHEMA_SUCCESS;
}

JsonValue* JsonValue_GetObjectProperty(const JsonValue* object, const char* key) {
    if (!object || object->type != JSON_TYPE_OBJECT || !key) {
        return NULL;
    }
    
    for (size_t i = 0; i < object->value.object_value.count; i++) {
        if (strcmp(object->value.object_value.keys[i], key) == 0) {
            return &object->value.object_value.values[i];
        }
    }
    
    return NULL;
}

JsonSchemaError JsonValue_AddArrayItem(JsonValue* array, JsonValue* item) {
    if (!array || array->type != JSON_TYPE_ARRAY || !item) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    size_t new_count = array->value.array_value.count + 1;
    array->value.array_value.items = (JsonValue*)realloc(array->value.array_value.items, new_count * sizeof(JsonValue));
    
    if (!array->value.array_value.items) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }
    
    array->value.array_value.items[new_count - 1] = *item;
    array->value.array_value.count = new_count;
    
    return JSONSCHEMA_SUCCESS;
}

SerializationResult* JsonSchema_Serialize(const JsonSchema* schema, const JsonValue* data) {
    if (!schema || !data) {
        SerializationResult* result = (SerializationResult*)calloc(1, sizeof(SerializationResult));
        if (result) {
            result->error = JSONSCHEMA_ERROR_INVALID_PARAM;
        }
        return result;
    }
    
    if (!g_library_initialized) {
        SerializationResult* result = (SerializationResult*)calloc(1, sizeof(SerializationResult));
        if (result) {
            result->error = JSONSCHEMA_ERROR_INVALID_PARAM;
        }
        return result;
    }
    
    // Validate data against schema
    JsonSchemaError validation_error = JsonSchema_Validate(schema, data);
    if (validation_error != JSONSCHEMA_SUCCESS) {
        SerializationResult* result = (SerializationResult*)calloc(1, sizeof(SerializationResult));
        if (result) {
            result->error = validation_error;
        }
        return result;
    }
    
    // Create result
    SerializationResult* result = (SerializationResult*)calloc(1, sizeof(SerializationResult));
    if (!result) {
        return NULL;
    }
    
    // Create binary data
    BinaryData* binary_data = BinaryData_Create(1024); // Start with 1KB
    if (!binary_data) {
        result->error = JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
        return result;
    }
    
    // Write schema key ID first
    BinarySerializer_WriteUInt32(binary_data, schema->key_id);
    
    // Serialize the data
    result->error = BinarySerializer_SerializeValue(schema, data, binary_data);
    
    if (result->error == JSONSCHEMA_SUCCESS) {
        result->data = *binary_data;
        free(binary_data); // Don't free the data, just the container
    } else {
        BinaryData_Free(binary_data);
    }
    
    return result;
}

JsonValue* JsonSchema_Deserialize(const JsonSchema* schema, const BinaryData* binary_data, JsonSchemaError* error) {
    if (!schema || !binary_data || !error) {
        if (error) *error = JSONSCHEMA_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    if (!g_library_initialized) {
        *error = JSONSCHEMA_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    size_t offset = 0;
    
    // Read and verify schema key ID
    uint32_t key_id = BinarySerializer_ReadUInt32(binary_data, &offset);
    if (key_id != schema->key_id) {
        *error = JSONSCHEMA_ERROR_DESERIALIZATION;
        return NULL;
    }
    
    // Deserialize the data
    JsonValue* result = NULL;
    *error = BinarySerializer_DeserializeValue(schema, binary_data, &offset, &result);
    
    return result;
}

JsonSchemaError JsonSchema_Validate(const JsonSchema* schema, const JsonValue* data) {
    if (!schema || !data) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    return JsonSchemaValidator_ValidateValue(schema, data);
}

const char* JsonSchema_GetErrorString(JsonSchemaError error) {
    switch (error) {
        case JSONSCHEMA_SUCCESS:
            return "Success";
        case JSONSCHEMA_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case JSONSCHEMA_ERROR_MEMORY_ALLOCATION:
            return "Memory allocation failed";
        case JSONSCHEMA_ERROR_INVALID_JSON:
            return "Invalid JSON";
        case JSONSCHEMA_ERROR_SCHEMA_VALIDATION:
            return "Schema validation failed";
        case JSONSCHEMA_ERROR_SERIALIZATION:
            return "Serialization failed";
        case JSONSCHEMA_ERROR_DESERIALIZATION:
            return "Deserialization failed";
        case JSONSCHEMA_ERROR_FILE_NOT_FOUND:
            return "File not found";
        case JSONSCHEMA_ERROR_INVALID_SCHEMA:
            return "Invalid schema";
        default:
            return "Unknown error";
    }
}

void SerializationResult_Free(SerializationResult* result) {
    if (!result) {
        return;
    }
    
    if (result->data.data) {
        free(result->data.data);
    }
    if (result->error_message) {
        free(result->error_message);
    }
    free(result);
}