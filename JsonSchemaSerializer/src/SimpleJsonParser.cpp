#include "SimpleJsonParser.h"
#include "../include/JsonSchemaSerializer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void SimpleJsonParser_SkipWhitespace(const char* json_string, size_t* offset) {
    if (!json_string || !offset) {
        return;
    }

    while (json_string[*offset] == ' ' || json_string[*offset] == '\t' || 
           json_string[*offset] == '\n' || json_string[*offset] == '\r') {
        (*offset)++;
    }
}

char* SimpleJsonParser_ReadString(const char* json_string, size_t* offset) {
    if (!json_string || !offset) {
        return NULL;
    }

    SimpleJsonParser_SkipWhitespace(json_string, offset);

    if (json_string[*offset] != '"') {
        return NULL;
    }

    (*offset)++; // Skip opening quote
    size_t start = *offset;

    while (json_string[*offset] != '"' && json_string[*offset] != '\0') {
        if (json_string[*offset] == '\\') {
            (*offset)++; // Skip escape character
        }
        (*offset)++;
    }

    if (json_string[*offset] != '"') {
        return NULL;
    }

    size_t length = *offset - start;
    char* result = (char*)malloc(length + 1);
    if (!result) {
        return NULL;
    }

    strncpy(result, json_string + start, length);
    result[length] = '\0';

    (*offset)++; // Skip closing quote
    return result;
}

int32_t SimpleJsonParser_ReadInteger(const char* json_string, size_t* offset) {
    if (!json_string || !offset) {
        return 0;
    }

    SimpleJsonParser_SkipWhitespace(json_string, offset);
    
    int32_t result = 0;
    int sign = 1;
    
    if (json_string[*offset] == '-') {
        sign = -1;
        (*offset)++;
    }
    
    while (json_string[*offset] >= '0' && json_string[*offset] <= '9') {
        result = result * 10 + (json_string[*offset] - '0');
        (*offset)++;
    }
    
    return result * sign;
}

JsonSchemaError SimpleJsonParser_ParseValue(const char* json_string, JsonValue** value) {
    if (!json_string || !value) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }

    size_t offset = 0;
    return SimpleJsonParser_ParseValueWithOffset(json_string, &offset, value);
}

JsonSchemaError SimpleJsonParser_ParseValueWithOffset(const char* json_string, size_t* offset, JsonValue** value) {
    if (!json_string || !offset || !value) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }

    SimpleJsonParser_SkipWhitespace(json_string, offset);

    *value = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!*value) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }

    char c = json_string[*offset];
    if (c == '"') {
        (*value)->type = JSON_TYPE_STRING;
        (*value)->value.string_value = SimpleJsonParser_ReadString(json_string, offset);
        if (!(*value)->value.string_value) {
            free(*value);
            *value = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }
    }
    else if (c == '{') {
        (*value)->type = JSON_TYPE_OBJECT;
        return SimpleJsonParser_ParseObjectWithOffset(json_string, offset, value);
    }
    else if (c == '[') {
        (*value)->type = JSON_TYPE_ARRAY;
        return SimpleJsonParser_ParseArrayWithOffset(json_string, offset, value);
    }
    else if (c == 't' || c == 'f') {
        (*value)->type = JSON_TYPE_BOOLEAN;
        if (strncmp(json_string + *offset, "true", 4) == 0) {
            (*value)->value.int_value = 1;
            *offset += 4;
        } else if (strncmp(json_string + *offset, "false", 5) == 0) {
            (*value)->value.int_value = 0;
            *offset += 5;
        } else {
            free(*value);
            *value = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }
    }
    else if (c == 'n') {
        (*value)->type = JSON_TYPE_NULL;
        if (strncmp(json_string + *offset, "null", 4) == 0) {
            *offset += 4;
        } else {
            free(*value);
            *value = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }
    }
    else if (c == '-' || (c >= '0' && c <= '9')) {
        // Simple integer parsing
        (*value)->type = JSON_TYPE_INTEGER;
        (*value)->value.int_value = SimpleJsonParser_ReadInteger(json_string, offset);
    }
    else {
        free(*value);
        *value = NULL;
        return JSONSCHEMA_ERROR_INVALID_JSON;
    }

    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError SimpleJsonParser_ParseObjectWithOffset(const char* json_string, size_t* offset, JsonValue** object) {
    if (!json_string || !offset || !object) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }

    SimpleJsonParser_SkipWhitespace(json_string, offset);

    if (json_string[*offset] != '{') {
        return JSONSCHEMA_ERROR_INVALID_JSON;
    }

    (*offset)++; // Skip opening brace
    SimpleJsonParser_SkipWhitespace(json_string, offset);

    *object = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!*object) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }

    (*object)->type = JSON_TYPE_OBJECT;
    (*object)->value.object_value.keys = NULL;
    (*object)->value.object_value.values = NULL;
    (*object)->value.object_value.count = 0;

    while (json_string[*offset] != '}' && json_string[*offset] != '\0') {
        char* key = SimpleJsonParser_ReadString(json_string, offset);
        if (!key) {
            JsonValue_Free(*object);
            *object = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }

        SimpleJsonParser_SkipWhitespace(json_string, offset);
        if (json_string[*offset] != ':') {
            free(key);
            JsonValue_Free(*object);
            *object = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }
        (*offset)++; // Skip colon
        SimpleJsonParser_SkipWhitespace(json_string, offset);

        JsonValue* value = NULL;
        if (SimpleJsonParser_ParseValueWithOffset(json_string, offset, &value) != JSONSCHEMA_SUCCESS) {
            free(key);
            JsonValue_Free(*object);
            *object = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }

        // Add to object
        size_t new_count = (*object)->value.object_value.count + 1;
        (*object)->value.object_value.keys = (char**)realloc((*object)->value.object_value.keys, new_count * sizeof(char*));
        (*object)->value.object_value.values = (JsonValue*)realloc((*object)->value.object_value.values, new_count * sizeof(JsonValue));
        
        (*object)->value.object_value.keys[new_count - 1] = key;
        (*object)->value.object_value.values[new_count - 1] = *value;
        (*object)->value.object_value.count = new_count;
        
        free(value);

        SimpleJsonParser_SkipWhitespace(json_string, offset);
        if (json_string[*offset] == ',') {
            (*offset)++; // Skip comma
            SimpleJsonParser_SkipWhitespace(json_string, offset);
        }
    }

    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError SimpleJsonParser_ParseArrayWithOffset(const char* json_string, size_t* offset, JsonValue** array) {
    if (!json_string || !offset || !array) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }

    SimpleJsonParser_SkipWhitespace(json_string, offset);

    if (json_string[*offset] != '[') {
        return JSONSCHEMA_ERROR_INVALID_JSON;
    }

    (*offset)++; // Skip opening bracket
    SimpleJsonParser_SkipWhitespace(json_string, offset);

    *array = (JsonValue*)calloc(1, sizeof(JsonValue));
    if (!*array) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }

    (*array)->type = JSON_TYPE_ARRAY;
    (*array)->value.array_value.items = NULL;
    (*array)->value.array_value.count = 0;

    while (json_string[*offset] != ']' && json_string[*offset] != '\0') {
        JsonValue* item = NULL;
        if (SimpleJsonParser_ParseValueWithOffset(json_string, offset, &item) != JSONSCHEMA_SUCCESS) {
            JsonValue_Free(*array);
            *array = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }

        // Add to array
        size_t new_count = (*array)->value.array_value.count + 1;
        (*array)->value.array_value.items = (JsonValue*)realloc((*array)->value.array_value.items, new_count * sizeof(JsonValue));
        (*array)->value.array_value.items[new_count - 1] = *item;
        (*array)->value.array_value.count = new_count;
        
        free(item);

        SimpleJsonParser_SkipWhitespace(json_string, offset);
        if (json_string[*offset] == ',') {
            (*offset)++; // Skip comma
            SimpleJsonParser_SkipWhitespace(json_string, offset);
        }
    }

    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError SimpleJsonParser_ParseSchema(const char* json_string, JsonSchema** schema) {
    if (!json_string || !schema) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }

    *schema = (JsonSchema*)calloc(1, sizeof(JsonSchema));
    if (!*schema) {
        return JSONSCHEMA_ERROR_MEMORY_ALLOCATION;
    }

    size_t offset = 0;
    SimpleJsonParser_SkipWhitespace(json_string, &offset);

    if (json_string[offset] != '{') {
        free(*schema);
        *schema = NULL;
        return JSONSCHEMA_ERROR_INVALID_JSON;
    }

    offset++; // Skip opening brace
    SimpleJsonParser_SkipWhitespace(json_string, &offset);

    while (json_string[offset] != '}' && json_string[offset] != '\0') {
        char* key = SimpleJsonParser_ReadString(json_string, &offset);
        if (!key) {
            free(*schema);
            *schema = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }

        SimpleJsonParser_SkipWhitespace(json_string, &offset);
        if (json_string[offset] != ':') {
            free(key);
            free(*schema);
            *schema = NULL;
            return JSONSCHEMA_ERROR_INVALID_JSON;
        }
        offset++; // Skip colon
        SimpleJsonParser_SkipWhitespace(json_string, &offset);

        if (strcmp(key, "$id") == 0) {
            (*schema)->id = SimpleJsonParser_ReadString(json_string, &offset);
        }
        else if (strcmp(key, "title") == 0) {
            (*schema)->title = SimpleJsonParser_ReadString(json_string, &offset);
        }
        else if (strcmp(key, "keyId") == 0) {
            (*schema)->key_id = (uint32_t)SimpleJsonParser_ReadInteger(json_string, &offset);
        }
        else if (strcmp(key, "type") == 0) {
            char* type_str = SimpleJsonParser_ReadString(json_string, &offset);
            if (strcmp(type_str, "object") == 0) {
                (*schema)->type = JSON_TYPE_OBJECT;
            }
            else if (strcmp(type_str, "array") == 0) {
                (*schema)->type = JSON_TYPE_ARRAY;
            }
            else if (strcmp(type_str, "string") == 0) {
                (*schema)->type = JSON_TYPE_STRING;
            }
            else if (strcmp(type_str, "integer") == 0) {
                (*schema)->type = JSON_TYPE_INTEGER;
            }
            else if (strcmp(type_str, "number") == 0) {
                (*schema)->type = JSON_TYPE_NUMBER;
            }
            else if (strcmp(type_str, "boolean") == 0) {
                (*schema)->type = JSON_TYPE_BOOLEAN;
            }
            free(type_str);
        }
        else if (strcmp(key, "required") == 0) {
            JsonValue* required_array = NULL;
            if (SimpleJsonParser_ParseArrayWithOffset(json_string, &offset, &required_array) == JSONSCHEMA_SUCCESS) {
                (*schema)->required.required_count = required_array->value.array_value.count;
                (*schema)->required.required_fields = (char**)malloc(required_array->value.array_value.count * sizeof(char*));
                for (size_t i = 0; i < required_array->value.array_value.count; i++) {
                    (*schema)->required.required_fields[i] = strdup(required_array->value.array_value.items[i].value.string_value);
                }
                JsonValue_Free(required_array);
            }
        }
        else if (strcmp(key, "properties") == 0) {
            JsonValue* properties_obj = NULL;
            if (SimpleJsonParser_ParseObjectWithOffset(json_string, &offset, &properties_obj) == JSONSCHEMA_SUCCESS) {
                (*schema)->properties.count = properties_obj->value.object_value.count;
                (*schema)->properties.keys = (char**)malloc(properties_obj->value.object_value.count * sizeof(char*));
                (*schema)->properties.values = (JsonValue*)malloc(properties_obj->value.object_value.count * sizeof(JsonValue));
                for (size_t i = 0; i < properties_obj->value.object_value.count; i++) {
                    (*schema)->properties.keys[i] = strdup(properties_obj->value.object_value.keys[i]);
                    (*schema)->properties.values[i] = properties_obj->value.object_value.values[i];
                }
                free(properties_obj->value.object_value.keys);
                free(properties_obj->value.object_value.values);
                free(properties_obj);
            }
        }
        else if (strcmp(key, "items") == 0) {
            JsonValue* items_obj = NULL;
            if (SimpleJsonParser_ParseObjectWithOffset(json_string, &offset, &items_obj) == JSONSCHEMA_SUCCESS) {
                (*schema)->array_items.count = 1;
                (*schema)->array_items.items = (JsonValue*)malloc(sizeof(JsonValue));
                (*schema)->array_items.items[0] = *items_obj;
                free(items_obj);
            }
        }
        else if (strcmp(key, "$ref") == 0) {
            (*schema)->ref = SimpleJsonParser_ReadString(json_string, &offset);
        }

        free(key);
        SimpleJsonParser_SkipWhitespace(json_string, &offset);
        
        if (json_string[offset] == ',') {
            offset++; // Skip comma
            SimpleJsonParser_SkipWhitespace(json_string, &offset);
        }
    }

    return JSONSCHEMA_SUCCESS;
}