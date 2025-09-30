#include "SimpleJsonSchemaValidator.h"
#include "../include/JsonSchemaSerializer.h"
#include <stdlib.h>
#include <string.h>

JsonSchemaError JsonSchemaValidator_Validate(const JsonValue* data, const JsonSchema* schema) {
    if (!data || !schema) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Simple validation - just check if data type matches schema type
    if (data->type != schema->type) {
        return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
    }
    
    // For objects, check required fields
    if (data->type == JSON_TYPE_OBJECT) {
        for (size_t i = 0; i < schema->required.required_count; i++) {
            const char* required_field = schema->required.required_fields[i];
            JsonValue* property = JsonValue_GetObjectProperty(data, required_field);
            if (!property) {
                return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
            }
        }
    }
    
    return JSONSCHEMA_SUCCESS;
}