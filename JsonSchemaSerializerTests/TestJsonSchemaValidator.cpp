#include "TestJsonSchemaValidator.h"
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int TestJsonSchemaValidator(void) {
    // Initialize library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Test schema validation
    const char* address_schema_json = 
        "{"
        "\"$id\": \"https://example.com/schemas/address.json\","
        "\"title\": \"Address\","
        "\"keyId\": 11,"
        "\"type\": \"object\","
        "\"properties\": {"
            "\"street\": {\"type\": \"string\"},"
            "\"city\": {\"type\": \"string\"},"
            "\"country\": {\"type\": \"string\"}"
        "},"
        "\"required\": [\"street\", \"city\"]"
        "}";
    
    JsonSchema* schema = NULL;
    error = JsonSchema_LoadFromString(address_schema_json, &schema);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(schema);
    
    // Test valid data
    JsonValue* valid_address = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(valid_address);
    
    JsonValue* street = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(street, "123 Main St");
    JsonValue_SetObjectProperty(valid_address, "street", street);
    
    JsonValue* city = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(city, "New York");
    JsonValue_SetObjectProperty(valid_address, "city", city);
    
    error = JsonSchema_Validate(schema, valid_address);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Test missing required field
    JsonValue* invalid_address = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(invalid_address);
    
    JsonValue* only_street = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(only_street, "123 Main St");
    JsonValue_SetObjectProperty(invalid_address, "street", only_street);
    // Missing required "city" field
    
    error = JsonSchema_Validate(schema, invalid_address);
    ASSERT_EQ(JSONSCHEMA_ERROR_SCHEMA_VALIDATION, error);
    
    // Test wrong type
    JsonValue* wrong_type_address = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(wrong_type_address);
    
    JsonValue* street_int = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(street_int, 123);
    JsonValue_SetObjectProperty(wrong_type_address, "street", street_int);
    
    JsonValue* city_str = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(city_str, "New York");
    JsonValue_SetObjectProperty(wrong_type_address, "city", city_str);
    
    error = JsonSchema_Validate(schema, wrong_type_address);
    ASSERT_EQ(JSONSCHEMA_ERROR_SCHEMA_VALIDATION, error);
    
    // Test array validation
    const char* array_schema_json = 
        "{"
        "\"keyId\": 12,"
        "\"type\": \"array\","
        "\"items\": {\"type\": \"string\"}"
        "}";
    
    JsonSchema* array_schema = NULL;
    error = JsonSchema_LoadFromString(array_schema_json, &array_schema);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    JsonValue* valid_array = JsonValue_Create(JSON_TYPE_ARRAY);
    ASSERT_NOT_NULL(valid_array);
    
    JsonValue* item1 = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(item1, "first");
    JsonValue_AddArrayItem(valid_array, item1);
    
    JsonValue* item2 = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(item2, "second");
    JsonValue_AddArrayItem(valid_array, item2);
    
    error = JsonSchema_Validate(array_schema, valid_array);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Test invalid array item type
    JsonValue* invalid_array = JsonValue_Create(JSON_TYPE_ARRAY);
    ASSERT_NOT_NULL(invalid_array);
    
    JsonValue* int_item = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(int_item, 123);
    JsonValue_AddArrayItem(invalid_array, int_item);
    
    error = JsonSchema_Validate(array_schema, invalid_array);
    ASSERT_EQ(JSONSCHEMA_ERROR_SCHEMA_VALIDATION, error);
    
    // Test primitive type validation
    const char* int_schema_json = "{\"keyId\": 13, \"type\": \"integer\"}";
    JsonSchema* int_schema = NULL;
    JsonSchema_LoadFromString(int_schema_json, &int_schema);
    
    JsonValue* valid_int = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(valid_int, 42);
    
    error = JsonSchema_Validate(int_schema, valid_int);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    JsonValue* invalid_int = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(invalid_int, "not a number");
    
    error = JsonSchema_Validate(int_schema, invalid_int);
    ASSERT_EQ(JSONSCHEMA_ERROR_SCHEMA_VALIDATION, error);
    
    // Cleanup
    JsonValue_Free(valid_address);
    JsonValue_Free(invalid_address);
    JsonValue_Free(wrong_type_address);
    JsonValue_Free(valid_array);
    JsonValue_Free(invalid_array);
    JsonValue_Free(valid_int);
    JsonValue_Free(invalid_int);
    
    JsonSchema_Free(schema);
    JsonSchema_Free(array_schema);
    JsonSchema_Free(int_schema);
    
    JsonSchemaSerializer_Cleanup();
    
    return 1;
}