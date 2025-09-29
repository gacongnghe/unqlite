#include <stdio.h>
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int main() {
    printf("Debug Test\n");
    printf("==========\n");
    
    // Initialize library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    printf("Initialize: %s\n", JsonSchema_GetErrorString(error));
    
    if (error != JSONSCHEMA_SUCCESS) {
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
        JsonValue_Free(value);
    }
    
    JsonSchemaSerializer_Cleanup();
    return 0;
}