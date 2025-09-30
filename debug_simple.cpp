#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copy the simple parser functions
typedef enum {
    JSONSCHEMA_SUCCESS = 0,
    JSONSCHEMA_ERROR_INVALID_PARAM = -1,
    JSONSCHEMA_ERROR_MEMORY_ALLOCATION = -2,
    JSONSCHEMA_ERROR_INVALID_JSON = -3,
    JSONSCHEMA_ERROR_SCHEMA_VALIDATION = -4,
    JSONSCHEMA_ERROR_SERIALIZATION = -5,
    JSONSCHEMA_ERROR_DESERIALIZATION = -6,
    JSONSCHEMA_ERROR_FILE_NOT_FOUND = -7,
    JSONSCHEMA_ERROR_INVALID_SCHEMA = -8
} JsonSchemaError;

typedef enum {
    JSON_TYPE_NULL = 0,
    JSON_TYPE_BOOLEAN = 1,
    JSON_TYPE_INTEGER = 2,
    JSON_TYPE_NUMBER = 3,
    JSON_TYPE_STRING = 4,
    JSON_TYPE_ARRAY = 5,
    JSON_TYPE_OBJECT = 6
} JsonValueType;

typedef struct JsonValue {
    JsonValueType type;
    union {
        int32_t int_value;
        double number_value;
        char* string_value;
        struct {
            JsonValue* items;
            size_t count;
        } array_value;
        struct {
            char** keys;
            JsonValue* values;
            size_t count;
        } object_value;
    } value;
} JsonValue;

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
        printf("Expected '\"' at position %zu, got '%c'\n", *offset, json_string[*offset]);
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
        printf("Expected closing '\"' at position %zu, got '%c'\n", *offset, json_string[*offset]);
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

int main() {
    printf("Simple Parser Debug Test\n");
    printf("========================\n");
    
    const char* test_json = "{\"type\": \"string\"}";
    printf("Testing JSON: %s\n", test_json);
    
    size_t offset = 0;
    SimpleJsonParser_SkipWhitespace(test_json, &offset);
    
    if (test_json[offset] != '{') {
        printf("Expected '{' at position %zu\n", offset);
        return 1;
    }
    offset++;
    
    SimpleJsonParser_SkipWhitespace(test_json, &offset);
    
    // Read key
    char* key = SimpleJsonParser_ReadString(test_json, &offset);
    if (!key) {
        printf("Failed to read key\n");
        return 1;
    }
    printf("Key: '%s'\n", key);
    free(key);
    
    // Skip colon
    SimpleJsonParser_SkipWhitespace(test_json, &offset);
    if (test_json[offset] != ':') {
        printf("Expected ':' at position %zu\n", offset);
        return 1;
    }
    offset++;
    
    SimpleJsonParser_SkipWhitespace(test_json, &offset);
    
    // Read value
    char* value = SimpleJsonParser_ReadString(test_json, &offset);
    if (!value) {
        printf("Failed to read value\n");
        return 1;
    }
    printf("Value: '%s'\n", value);
    free(value);
    
    // Skip closing brace
    SimpleJsonParser_SkipWhitespace(test_json, &offset);
    if (test_json[offset] != '}') {
        printf("Expected '}' at position %zu, got '%c'\n", offset, test_json[offset]);
        return 1;
    }
    
    printf("Success!\n");
    return 0;
}