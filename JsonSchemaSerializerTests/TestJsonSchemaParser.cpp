#include "TestJsonSchemaParser.h"
#include "../JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int TestJsonSchemaParser(void) {
    // Initialize library first
    JsonSchemaError init_error = JsonSchemaSerializer_Initialize();
    if (init_error != JSONSCHEMA_SUCCESS) {
        printf("Library initialization failed: %s\n", JsonSchema_GetErrorString(init_error));
        return 0;
    }
    
    // Test schema loading from string
    const char* address_schema_json = 
        "{"
        "\"$id\": \"https://example.com/schemas/address.json\","
        "\"$schema\": \"http://json-schema.org/draft-07/schema#\","
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
    JsonSchemaError error = JsonSchema_LoadFromString(address_schema_json, &schema);
    
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Schema loading failed: %s\n", JsonSchema_GetErrorString(error));
        return 0;
    }
    
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_NOT_NULL(schema);
    ASSERT_STR_EQ("https://example.com/schemas/address.json", schema->id);
    ASSERT_STR_EQ("Address", schema->title);
    ASSERT_EQ(11, schema->key_id);
    ASSERT_EQ(JSON_TYPE_OBJECT, schema->type);
    ASSERT_EQ(2, schema->required.required_count);
    ASSERT_STR_EQ("street", schema->required.required_fields[0]);
    ASSERT_STR_EQ("city", schema->required.required_fields[1]);
    ASSERT_EQ(3, schema->properties.count);
    
    // Test JSON value creation and manipulation
    JsonValue* value = JsonValue_Create(JSON_TYPE_STRING);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(JSON_TYPE_STRING, value->type);
    
    error = JsonValue_SetString(value, "123 Main St");
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_STR_EQ("123 Main St", value->value.string_value);
    
    // Test object creation
    JsonValue* object = JsonValue_Create(JSON_TYPE_OBJECT);
    ASSERT_NOT_NULL(object);
    
    JsonValue* street_value = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(street_value, "123 Main St");
    
    error = JsonValue_SetObjectProperty(object, "street", street_value);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    
    JsonValue* retrieved = JsonValue_GetObjectProperty(object, "street");
    ASSERT_NOT_NULL(retrieved);
    ASSERT_STR_EQ("123 Main St", retrieved->value.string_value);
    
    // Test array creation
    JsonValue* array = JsonValue_Create(JSON_TYPE_ARRAY);
    ASSERT_NOT_NULL(array);
    
    JsonValue* item1 = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(item1, "item1");
    
    error = JsonValue_AddArrayItem(array, item1);
    ASSERT_EQ(JSONSCHEMA_SUCCESS, error);
    ASSERT_EQ(1, array->value.array_value.count);
    ASSERT_STR_EQ("item1", array->value.array_value.items[0].value.string_value);
    
    // Cleanup
    JsonValue_Free(value);
    JsonValue_Free(object);
    JsonValue_Free(array);
    JsonSchema_Free(schema);
    
    JsonSchemaSerializer_Cleanup();
    
    return 1;
}