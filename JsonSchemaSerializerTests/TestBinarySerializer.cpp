#include "TestBinarySerializer.h"
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int TestBinarySerializer(void) {
    // Initialize library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    // Test basic serialization/deserialization
    const char* address_schema_json = 
        "{"
        "\"$id\": \"https://example.com/schemas/address.json\","
        "\"title\": \"Address\","
        "\"keyId\": 11,"
        "\"type\": \"object\","
        "\"properties\": {"
            "\"street\": {\"type\": \"string\"},"
            "\"city\": {\"type\": \"string\"}"
        "},"
        "\"required\": [\"street\", \"city\"]"
        "}";
    
    JsonSchema* schema = NULL;
    error = JsonSchema_LoadFromString(address_schema_json, &schema);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(schema);
    
    // Create test data
    JsonValue* address = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(address);
    
    JsonValue* street = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(street, "123 Main St");
    JsonValue_SetObjectProperty(address, "street", street);
    
    JsonValue* city = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(city, "New York");
    JsonValue_SetObjectProperty(address, "city", city);
    
    // Serialize
    SerializationResult* result = JsonSchema_Serialize(schema, address);
    ASSERT_NOT_NULL(result);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, result->error);
    ASSERT_TRUE(result->data.size > 0);
    ASSERT_NOT_NULL(result->data.data);
    
    // Deserialize
    JsonValue* deserialized = JsonSchema_Deserialize(schema, &result->data, &error);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(deserialized);
    
    // Verify deserialized data
    JsonValue* deserialized_street = JsonValue_GetObjectProperty(deserialized, "street");
    ASSERT_NOT_NULL(deserialized_street);
    ASSERT_EQ(JSON_TYPE_STRING, deserialized_street->type);
    ASSERT_STR_EQ("123 Main St", deserialized_street->value.string_value);
    
    JsonValue* deserialized_city = JsonValue_GetObjectProperty(deserialized, "city");
    ASSERT_NOT_NULL(deserialized_city);
    ASSERT_EQ(JSON_TYPE_STRING, deserialized_city->type);
    ASSERT_STR_EQ("New York", deserialized_city->value.string_value);
    
    // Test array serialization
    const char* array_schema_json = 
        "{"
        "\"keyId\": 12,"
        "\"type\": \"array\","
        "\"items\": {\"type\": \"string\"}"
        "}";
    
    JsonSchema* array_schema = NULL;
    error = JsonSchema_LoadFromString(array_schema_json, &array_schema);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    JsonValue* string_array = JsonValue_Create(JSON_TYPE_ARRAY);
    ASSERT_NOT_NULL(string_array);
    
    JsonValue* item1 = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(item1, "first");
    JsonValue_AddArrayItem(string_array, item1);
    
    JsonValue* item2 = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(item2, "second");
    JsonValue_AddArrayItem(string_array, item2);
    
    SerializationResult* array_result = JsonSchema_Serialize(array_schema, string_array);
    ASSERT_NOT_NULL(array_result);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, array_result->error);
    
    JsonValue* deserialized_array = JsonSchema_Deserialize(array_schema, &array_result->data, &error);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(deserialized_array);
    ASSERT_EQ(2, deserialized_array->value.array_value.count);
    ASSERT_STR_EQ("first", deserialized_array->value.array_value.items[0].value.string_value);
    ASSERT_STR_EQ("second", deserialized_array->value.array_value.items[1].value.string_value);
    
    // Test primitive types
    JsonValue* int_value = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(int_value, 42);
    
    const char* int_schema_json = "{\"keyId\": 13, \"type\": \"integer\"}";
    JsonSchema* int_schema = NULL;
    JsonSchema_LoadFromString(int_schema_json, &int_schema);
    
    SerializationResult* int_result = JsonSchema_Serialize(int_schema, int_value);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, int_result->error);
    
    JsonValue* deserialized_int = JsonSchema_Deserialize(int_schema, &int_result->data, &error);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_EQ(42, deserialized_int->value.int_value);
    
    // Cleanup
    JsonValue_Free(address);
    JsonValue_Free(deserialized);
    JsonValue_Free(string_array);
    JsonValue_Free(deserialized_array);
    JsonValue_Free(int_value);
    JsonValue_Free(deserialized_int);
    
    JsonSchema_Free(schema);
    JsonSchema_Free(array_schema);
    JsonSchema_Free(int_schema);
    
    SerializationResult_Free(result);
    SerializationResult_Free(array_result);
    SerializationResult_Free(int_result);
    
    JsonSchemaSerializer_Cleanup();
    
    return 1;
}