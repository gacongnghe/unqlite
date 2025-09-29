#include "TestRunner.h"
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int TestJsonSchemaSerializer(void) {
    // Initialize library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Test basic serialization/deserialization workflow
    const char* simple_schema_json = 
        "{"
        "\"$id\": \"https://example.com/schemas/simple.json\","
        "\"title\": \"Simple\","
        "\"keyId\": 99,"
        "\"type\": \"object\","
        "\"properties\": {"
            "\"name\": {\"type\": \"string\"},"
            "\"age\": {\"type\": \"integer\"}"
        "},"
        "\"required\": [\"name\"]"
        "}";
    
    JsonSchema* schema = NULL;
    error = JsonSchema_LoadFromString(simple_schema_json, &schema);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(schema);
    
    // Create test data
    JsonValue* person = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(person);
    
    JsonValue* name = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(name, "Alice");
    JsonValue_SetObjectProperty(person, "name", name);
    
    JsonValue* age = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(age, 30);
    JsonValue_SetObjectProperty(person, "age", age);
    
    // Validate
    error = JsonSchema_Validate(schema, person);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Serialize
    SerializationResult* result = JsonSchema_Serialize(schema, person);
    ASSERT_NOT_NULL(result);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, result->error);
    ASSERT_TRUE(result->data.size > 0);
    
    // Deserialize
    JsonValue* deserialized = JsonSchema_Deserialize(schema, &result->data, &error);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(deserialized);
    
    // Verify deserialized data
    JsonValue* deserialized_name = JsonValue_GetObjectProperty(deserialized, "name");
    ASSERT_NOT_NULL(deserialized_name);
    ASSERT_STR_EQ("Alice", deserialized_name->value.string_value);
    
    JsonValue* deserialized_age = JsonValue_GetObjectProperty(deserialized, "age");
    ASSERT_NOT_NULL(deserialized_age);
    ASSERT_EQ(30, deserialized_age->value.int_value);
    
    // Test error handling
    JsonValue* invalid_person = JsonValue_Create(JSON_TYPE_OBJECT);
    // Missing required "name" field
    JsonValue* invalid_age = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(invalid_age, 25);
    JsonValue_SetObjectProperty(invalid_person, "age", invalid_age);
    
    error = JsonSchema_Validate(schema, invalid_person);
    ASSERT_EQ(JSONSCHEMA_ERROR_SCHEMA_VALIDATION, error);
    
    // Test error string function
    const char* error_string = JsonSchema_GetErrorString(JSONSCHEMA_SUCCESS);
    ASSERT_STR_EQ("Success", error_string);
    
    error_string = JsonSchema_GetErrorString(JSONSCHEMA_ERROR_INVALID_PARAM);
    ASSERT_STR_EQ("Invalid parameter", error_string);
    
    // Cleanup
    JsonValue_Free(person);
    JsonValue_Free(deserialized);
    JsonValue_Free(invalid_person);
    JsonSchema_Free(schema);
    SerializationResult_Free(result);
    
    JsonSchemaSerializer_Cleanup();
    
    return 1;
}