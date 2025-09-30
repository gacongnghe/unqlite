#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test the ReadString function directly
char* TestReadString(const char* json_string, size_t* offset) {
    if (!json_string || !offset) {
        return NULL;
    }

    // Skip whitespace
    while (json_string[*offset] == ' ' || json_string[*offset] == '\t' || 
           json_string[*offset] == '\n' || json_string[*offset] == '\r') {
        (*offset)++;
    }

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
    printf("Minimal JSON Test\n");
    printf("=================\n");
    
    const char* test_json = "{\"type\": \"string\"}";
    printf("Testing JSON: %s\n", test_json);
    
    size_t offset = 0;
    
    // Skip opening brace
    if (test_json[offset] != '{') {
        printf("Expected '{' at position %zu\n", offset);
        return 1;
    }
    offset++;
    
    // Read first string
    char* key = TestReadString(test_json, &offset);
    if (!key) {
        printf("Failed to read key\n");
        return 1;
    }
    printf("Key: '%s'\n", key);
    free(key);
    
    // Skip colon
    if (test_json[offset] != ':') {
        printf("Expected ':' at position %zu\n", offset);
        return 1;
    }
    offset++;
    
    // Read second string
    char* value = TestReadString(test_json, &offset);
    if (!value) {
        printf("Failed to read value\n");
        return 1;
    }
    printf("Value: '%s'\n", value);
    free(value);
    
    // Skip closing brace
    if (test_json[offset] != '}') {
        printf("Expected '}' at position %zu\n", offset);
        return 1;
    }
    
    printf("Success!\n");
    return 0;
}