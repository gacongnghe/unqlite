#include "JsonSchemaSerializer.h"
#include "MinimalJsonParser.h"
#include "SimpleBinarySerializer.h"
#include "SimpleJsonSchemaValidator.h"
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
    
    // Parse the JSON schema
    JsonSchemaError result = MinimalJsonParser_ParseSchema(json_string, schema);
    free(json_string);
    
    return result;
}

JsonSchemaError JsonSchema_ParseFromString(const char* json_string, JsonSchema** schema) {
    if (!json_string || !schema) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    return MinimalJsonParser_ParseSchema(json_string, schema);
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
    if (schema->required.required_fields) {
        for (size_t i = 0; i < schema->required.required_count; i++) {
            free(schema->required.required_fields[i]);
        }
        free(schema->required.required_fields);
    }
    
    // Free properties
    if (schema->properties.keys) {
        for (size_t i = 0; i < schema->properties.count; i++) {
            free(schema->properties.keys[i]);
            JsonValue_Free(&schema->properties.values[i]);
        }
        free(schema->properties.keys);
        free(schema->properties.values);
    }
    
    // Free array items
    if (schema->array_items.items) {
        for (size_t i = 0; i < schema->array_items.count; i++) {
            JsonValue_Free(&schema->array_items.items[i]);
        }
        free(schema->array_items.items);
    }
    
    free(schema);
}

JsonSchemaError JsonSchema_Validate(const JsonSchema* schema, const JsonValue* data) {
    if (!schema || !data) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    return JsonSchemaValidator_Validate(data, schema);
}

JsonSchemaError JsonSchemaSerializer_Serialize(const JsonValue* data, const JsonSchema* schema, uint8_t** buffer, size_t* buffer_size) {
    if (!data || !schema || !buffer || !buffer_size) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Validate the data against the schema first
    JsonSchemaError validation_error = JsonSchemaValidator_Validate(data, schema);
    if (validation_error != JSONSCHEMA_SUCCESS) {
        return validation_error;
    }
    
    // Use the simple binary serializer
    return BinarySerializer_Serialize(data, schema, buffer, buffer_size);
}

JsonSchemaError JsonSchemaSerializer_Deserialize(const uint8_t* buffer, size_t buffer_size, const JsonSchema* schema, JsonValue** data) {
    if (!buffer || !schema || !data) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    if (!g_library_initialized) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Use the simple binary serializer
    return BinarySerializer_Deserialize(buffer, buffer_size, schema, data);
}

// JsonValue utility functions
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
            if (value->value.array_value.items) {
                for (size_t i = 0; i < value->value.array_value.count; i++) {
                    JsonValue_Free(&value->value.array_value.items[i]);
                }
                free(value->value.array_value.items);
            }
            break;
        case JSON_TYPE_OBJECT:
            if (value->value.object_value.keys) {
                for (size_t i = 0; i < value->value.object_value.count; i++) {
                    free(value->value.object_value.keys[i]);
                    JsonValue_Free(&value->value.object_value.values[i]);
                }
                free(value->value.object_value.keys);
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
            break;
        case JSON_TYPE_BOOLEAN:
        case JSON_TYPE_INTEGER:
            copy->value.int_value = value->value.int_value;
            break;
        case JSON_TYPE_NUMBER:
            copy->value.number_value = value->value.number_value;
            break;
        case JSON_TYPE_STRING:
            copy->value.string_value = value->value.string_value ? strdup(value->value.string_value) : NULL;
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

// JsonValue setter functions
JsonSchemaError JsonValue_SetString(JsonValue* value, const char* str) {
    if (!value) return JSONSCHEMA_ERROR_INVALID_PARAM;
    value->type = JSON_TYPE_STRING;
    if (value->value.string_value) {
        free(value->value.string_value);
    }
    value->value.string_value = str ? strdup(str) : NULL;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetInteger(JsonValue* value, int32_t int_val) {
    if (!value) return JSONSCHEMA_ERROR_INVALID_PARAM;
    value->type = JSON_TYPE_INTEGER;
    value->value.int_value = int_val;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetNumber(JsonValue* value, double number_val) {
    if (!value) return JSONSCHEMA_ERROR_INVALID_PARAM;
    value->type = JSON_TYPE_NUMBER;
    value->value.number_value = number_val;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetBoolean(JsonValue* value, int32_t bool_val) {
    if (!value) return JSONSCHEMA_ERROR_INVALID_PARAM;
    value->type = JSON_TYPE_BOOLEAN;
    value->value.int_value = bool_val ? 1 : 0;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetNull(JsonValue* value) {
    if (!value) return JSONSCHEMA_ERROR_INVALID_PARAM;
    value->type = JSON_TYPE_NULL;
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonValue_SetObjectProperty(JsonValue* value, const char* key, JsonValue* prop_value) {
    if (!value || !key) return JSONSCHEMA_ERROR_INVALID_PARAM;
    
    if (value->type != JSON_TYPE_OBJECT) {
        value->type = JSON_TYPE_OBJECT;
        value->value.object_value.keys = NULL;
        value->value.object_value.values = NULL;
        value->value.object_value.count = 0;
    }
    
    // Check if key already exists
    for (size_t i = 0; i < value->value.object_value.count; i++) {
        if (strcmp(value->value.object_value.keys[i], key) == 0) {
            // Update existing property
            JsonValue_Free(&value->value.object_value.values[i]);
            value->value.object_value.values[i] = *JsonValue_Copy(prop_value);
            return JSONSCHEMA_SUCCESS;
        }
    }
    
    // Add new property
    size_t new_count = value->value.object_value.count + 1;
    value->value.object_value.keys = (char**)realloc(value->value.object_value.keys, new_count * sizeof(char*));
    value->value.object_value.values = (JsonValue*)realloc(value->value.object_value.values, new_count * sizeof(JsonValue));
    
    value->value.object_value.keys[new_count - 1] = strdup(key);
    value->value.object_value.values[new_count - 1] = *JsonValue_Copy(prop_value);
    value->value.object_value.count = new_count;
    return JSONSCHEMA_SUCCESS;
}

JsonValue* JsonValue_GetObjectProperty(const JsonValue* value, const char* key) {
    if (!value || !key || value->type != JSON_TYPE_OBJECT) {
        return NULL;
    }
    
    for (size_t i = 0; i < value->value.object_value.count; i++) {
        if (strcmp(value->value.object_value.keys[i], key) == 0) {
            return &value->value.object_value.values[i];
        }
    }
    
    return NULL;
}

JsonSchemaError JsonValue_AddArrayItem(JsonValue* value, JsonValue* item) {
    if (!value || !item) return JSONSCHEMA_ERROR_INVALID_PARAM;
    
    if (value->type != JSON_TYPE_ARRAY) {
        value->type = JSON_TYPE_ARRAY;
        value->value.array_value.items = NULL;
        value->value.array_value.count = 0;
    }
    
    size_t new_count = value->value.array_value.count + 1;
    value->value.array_value.items = (JsonValue*)realloc(value->value.array_value.items, new_count * sizeof(JsonValue));
    value->value.array_value.items[new_count - 1] = *JsonValue_Copy(item);
    value->value.array_value.count = new_count;
    return JSONSCHEMA_SUCCESS;
}

// Additional missing functions
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

JsonSchemaError JsonSchema_LoadFromString(const char* json_string, JsonSchema** schema) {
    return JsonSchema_ParseFromString(json_string, schema);
}

JsonValue* JsonValue_Create(JsonValueType type) {
    JsonValue* value = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!value) {
        return NULL;
    }
    value->type = type;
    return value;
}

SerializationResult* JsonSchema_Serialize(const JsonSchema* schema, const JsonValue* data) {
    if (!schema || !data) {
        return NULL;
    }
    
    SerializationResult* result = (SerializationResult*)calloc(1, sizeof(SerializationResult));
    if (!result) {
        return NULL;
    }
    
    // Use the simple binary serializer
    uint8_t* buffer = NULL;
    size_t buffer_size = 0;
    result->error = BinarySerializer_Serialize(data, schema, &buffer, &buffer_size);
    
    if (result->error == JSONSCHEMA_SUCCESS) {
        result->data.data = buffer;
        result->data.size = buffer_size;
    }
    
    return result;
}

JsonValue* JsonSchema_Deserialize(const JsonSchema* schema, const BinaryData* binary_data, JsonSchemaError* error) {
    if (!schema || !binary_data || !error) {
        if (error) *error = JSONSCHEMA_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    JsonValue* data = NULL;
    *error = BinarySerializer_Deserialize(binary_data->data, binary_data->size, schema, &data);
    
    return data;
}

void SerializationResult_Free(SerializationResult* result) {
    if (!result) return;
    if (result->data.data) {
        free(result->data.data);
    }
    free(result);
}