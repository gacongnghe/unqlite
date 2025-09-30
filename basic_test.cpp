#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the library headers
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int main() {
    printf("Basic JSON Test\n");
    printf("===============\n");
    
    // Initialize library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Library initialization failed: %s\n", JsonSchema_GetErrorString(error));
        return 1;
    }
    
    // Test simple JSON parsing
    const char* simple_json = "{\"type\": \"string\"}";
    printf("Testing JSON: %s\n", simple_json);
    
    JsonValue* value = NULL;
    error = JsonSchemaParser_ParseValue(simple_json, &value);
    printf("Parse result: %s\n", JsonSchema_GetErrorString(error));
    
    if (value) {
        printf("Value type: %d\n", value->type);
        if (value->type == JSON_TYPE_OBJECT) {
            printf("Object has %zu properties\n", value->value.object_value.count);
            for (size_t i = 0; i < value->value.object_value.count; i++) {
                printf("  Property %zu: %s\n", i, value->value.object_value.keys[i]);
            }
        }
        JsonValue_Free(value);
    }
    
    JsonSchemaSerializer_Cleanup();
    return 0;
}