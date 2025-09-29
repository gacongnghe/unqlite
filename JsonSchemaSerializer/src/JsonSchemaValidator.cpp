#include "JsonSchemaValidator.h"
#include <string.h>

JsonSchemaError JsonSchemaValidator_ValidateValue(const JsonSchema* schema, const JsonValue* value) {
    if (!schema || !value) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Check type compatibility
    if (!JsonSchemaValidator_IsValidType(schema->type, value->type)) {
        return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
    }
    
    switch (schema->type) {
        case JSON_TYPE_OBJECT:
            return JsonSchemaValidator_ValidateObject(schema, value);
            
        case JSON_TYPE_ARRAY:
            return JsonSchemaValidator_ValidateArray(schema, value);
            
        case JSON_TYPE_STRING:
            return JsonSchemaValidator_ValidateString(schema, value);
            
        case JSON_TYPE_INTEGER:
            return JsonSchemaValidator_ValidateInteger(schema, value);
            
        case JSON_TYPE_NUMBER:
            return JsonSchemaValidator_ValidateNumber(schema, value);
            
        case JSON_TYPE_BOOLEAN:
        case JSON_TYPE_NULL:
            // These types are always valid if type matches
            return JSONSCHEMA_SUCCESS;
            
        default:
            return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
    }
}

JsonSchemaError JsonSchemaValidator_ValidateObject(const JsonSchema* schema, const JsonValue* object) {
    if (!schema || !object || object->type != JSON_TYPE_OBJECT) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Validate required fields
    JsonSchemaError error = JsonSchemaValidator_ValidateRequiredFields(schema, object);
    if (error != JSONSCHEMA_SUCCESS) {
        return error;
    }
    
    // Validate each property
    for (size_t i = 0; i < object->value.object_value.count; i++) {
        const char* key = object->value.object_value.keys[i];
        const JsonValue* value = &object->value.object_value.values[i];
        
        // Find corresponding schema for this property
        JsonValue* property_schema = NULL;
        int found = 0;
        for (size_t j = 0; j < schema->properties.count; j++) {
            if (strcmp(schema->properties.keys[j], key) == 0) {
                property_schema = &schema->properties.values[j];
                found = 1;
                break;
            }
        }
        
        if (found && property_schema) {
            // Create a temporary schema for validation
            JsonSchema temp_schema = {0};
            temp_schema.type = property_schema->type;
            error = JsonSchemaValidator_ValidateValue(&temp_schema, value);
            if (error != JSONSCHEMA_SUCCESS) {
                return error;
            }
        }
        // If property not found in schema, it's still valid (additional properties allowed)
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonSchemaValidator_ValidateArray(const JsonSchema* schema, const JsonValue* array) {
    if (!schema || !array || array->type != JSON_TYPE_ARRAY) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Validate each item in the array
    for (size_t i = 0; i < array->value.array_value.count; i++) {
        JsonSchemaError error = JsonSchemaValidator_ValidateValue(schema, &array->value.array_value.items[i]);
        if (error != JSONSCHEMA_SUCCESS) {
            return error;
        }
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonSchemaValidator_ValidateString(const JsonSchema* schema, const JsonValue* value) {
    if (!schema || !value || value->type != JSON_TYPE_STRING) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Basic string validation - can be extended with minLength, maxLength, pattern, etc.
    if (!value->value.string_value) {
        return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
    }
    
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonSchemaValidator_ValidateInteger(const JsonSchema* schema, const JsonValue* value) {
    if (!schema || !value || value->type != JSON_TYPE_INTEGER) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Basic integer validation - can be extended with minimum, maximum, etc.
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonSchemaValidator_ValidateNumber(const JsonSchema* schema, const JsonValue* value) {
    if (!schema || !value || value->type != JSON_TYPE_NUMBER) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Basic number validation - can be extended with minimum, maximum, etc.
    return JSONSCHEMA_SUCCESS;
}

JsonSchemaError JsonSchemaValidator_ValidateRequiredFields(const JsonSchema* schema, const JsonValue* object) {
    if (!schema || !object || object->type != JSON_TYPE_OBJECT) {
        return JSONSCHEMA_ERROR_INVALID_PARAM;
    }
    
    // Check each required field
    for (size_t i = 0; i < schema->required.required_count; i++) {
        const char* required_field = schema->required.required_fields[i];
        int found = 0;
        
        // Look for the required field in the object
        for (size_t j = 0; j < object->value.object_value.count; j++) {
            if (strcmp(object->value.object_value.keys[j], required_field) == 0) {
                found = 1;
                break;
            }
        }
        
        if (!found) {
            return JSONSCHEMA_ERROR_SCHEMA_VALIDATION;
        }
    }
    
    return JSONSCHEMA_SUCCESS;
}

int32_t JsonSchemaValidator_IsValidType(JsonValueType schema_type, JsonValueType value_type) {
    // Allow some type flexibility
    if (schema_type == value_type) {
        return 1;
    }
    
    // Allow integer to be treated as number
    if (schema_type == JSON_TYPE_NUMBER && value_type == JSON_TYPE_INTEGER) {
        return 1;
    }
    
    return 0;
}