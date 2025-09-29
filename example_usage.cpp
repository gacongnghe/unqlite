#include <stdio.h>
#include <stdlib.h>
#include "JsonSchemaSerializer/include/JsonSchemaSerializer.h"

int main() {
    printf("JsonSchemaSerializer Example Usage\n");
    printf("==================================\n\n");
    
    // Initialize the library
    JsonSchemaError error = JsonSchemaSerializer_Initialize();
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Failed to initialize library: %s\n", JsonSchema_GetErrorString(error));
        return 1;
    }
    
    // Load address schema
    printf("Loading address schema...\n");
    JsonSchema* address_schema = NULL;
    error = JsonSchema_LoadFromFile("address.schema.json", &address_schema);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Failed to load address schema: %s\n", JsonSchema_GetErrorString(error));
        JsonSchemaSerializer_Cleanup();
        return 1;
    }
    
    printf("Address schema loaded successfully!\n");
    printf("  Title: %s\n", address_schema->title);
    printf("  Key ID: %u\n", address_schema->key_id);
    printf("  Required fields: %zu\n", address_schema->required.required_count);
    
    // Create address data
    printf("\nCreating address data...\n");
    JsonValue* address = JsonValue_Create(JSON_TYPE_OBJECT);
    
    JsonValue* street = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(street, "123 Main Street");
    JsonValue_SetObjectProperty(address, "street", street);
    
    JsonValue* city = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(city, "New York");
    JsonValue_SetObjectProperty(address, "city", city);
    
    JsonValue* country = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(country, "USA");
    JsonValue_SetObjectProperty(address, "country", country);
    
    // Validate address data
    printf("Validating address data...\n");
    error = JsonSchema_Validate(address_schema, address);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Address validation failed: %s\n", JsonSchema_GetErrorString(error));
    } else {
        printf("Address validation passed!\n");
    }
    
    // Serialize address
    printf("Serializing address...\n");
    SerializationResult* address_result = JsonSchema_Serialize(address_schema, address);
    if (address_result->error != JSONSCHEMA_SUCCESS) {
        printf("Address serialization failed: %s\n", JsonSchema_GetErrorString(address_result->error));
    } else {
        printf("Address serialized successfully! Size: %zu bytes\n", address_result->data.size);
    }
    
    // Deserialize address
    printf("Deserializing address...\n");
    JsonValue* deserialized_address = JsonSchema_Deserialize(address_schema, &address_result->data, &error);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Address deserialization failed: %s\n", JsonSchema_GetErrorString(error));
    } else {
        printf("Address deserialized successfully!\n");
        
        JsonValue* deserialized_street = JsonValue_GetObjectProperty(deserialized_address, "street");
        JsonValue* deserialized_city = JsonValue_GetObjectProperty(deserialized_address, "city");
        JsonValue* deserialized_country = JsonValue_GetObjectProperty(deserialized_address, "country");
        
        printf("  Street: %s\n", deserialized_street ? deserialized_street->value.string_value : "NULL");
        printf("  City: %s\n", deserialized_city ? deserialized_city->value.string_value : "NULL");
        printf("  Country: %s\n", deserialized_country ? deserialized_country->value.string_value : "NULL");
    }
    
    // Load person schema
    printf("\nLoading person schema...\n");
    JsonSchema* person_schema = NULL;
    error = JsonSchema_LoadFromFile("person.schema.json", &person_schema);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Failed to load person schema: %s\n", JsonSchema_GetErrorString(error));
    } else {
        printf("Person schema loaded successfully!\n");
        printf("  Title: %s\n", person_schema->title);
        printf("  Key ID: %u\n", person_schema->key_id);
        printf("  Required fields: %zu\n", person_schema->required.required_count);
    }
    
    // Create person data
    printf("\nCreating person data...\n");
    JsonValue* person = JsonValue_Create(JSON_TYPE_OBJECT);
    
    JsonValue* id = JsonValue_Create(JSON_TYPE_INTEGER);
    JsonValue_SetInteger(id, 12345);
    JsonValue_SetObjectProperty(person, "id", id);
    
    JsonValue* name = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(name, "John Doe");
    JsonValue_SetObjectProperty(person, "name", name);
    
    // Add address to person (reuse the address we created)
    JsonValue* address_copy = JsonValue_Copy(address);
    JsonValue_SetObjectProperty(person, "address", address_copy);
    
    // Create phone numbers array
    JsonValue* phones = JsonValue_Create(JSON_TYPE_ARRAY);
    
    JsonValue* phone1 = JsonValue_Create(JSON_TYPE_OBJECT);
    JsonValue* phone1_number = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(phone1_number, "+1-555-123-4567");
    JsonValue_SetObjectProperty(phone1, "number", phone1_number);
    
    JsonValue* phone1_type = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(phone1_type, "mobile");
    JsonValue_SetObjectProperty(phone1, "type", phone1_type);
    
    JsonValue_AddArrayItem(phones, phone1);
    
    JsonValue* phone2 = JsonValue_Create(JSON_TYPE_OBJECT);
    JsonValue* phone2_number = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(phone2_number, "+1-555-987-6543");
    JsonValue_SetObjectProperty(phone2, "number", phone2_number);
    
    JsonValue* phone2_type = JsonValue_Create(JSON_TYPE_STRING);
    JsonValue_SetString(phone2_type, "home");
    JsonValue_SetObjectProperty(phone2, "type", phone2_type);
    
    JsonValue_AddArrayItem(phones, phone2);
    
    JsonValue_SetObjectProperty(person, "phones", phones);
    
    // Validate person data
    printf("Validating person data...\n");
    error = JsonSchema_Validate(person_schema, person);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Person validation failed: %s\n", JsonSchema_GetErrorString(error));
    } else {
        printf("Person validation passed!\n");
    }
    
    // Serialize person
    printf("Serializing person...\n");
    SerializationResult* person_result = JsonSchema_Serialize(person_schema, person);
    if (person_result->error != JSONSCHEMA_SUCCESS) {
        printf("Person serialization failed: %s\n", JsonSchema_GetErrorString(person_result->error));
    } else {
        printf("Person serialized successfully! Size: %zu bytes\n", person_result->data.size);
    }
    
    // Deserialize person
    printf("Deserializing person...\n");
    JsonValue* deserialized_person = JsonSchema_Deserialize(person_schema, &person_result->data, &error);
    if (error != JSONSCHEMA_SUCCESS) {
        printf("Person deserialization failed: %s\n", JsonSchema_GetErrorString(error));
    } else {
        printf("Person deserialized successfully!\n");
        
        JsonValue* deserialized_id = JsonValue_GetObjectProperty(deserialized_person, "id");
        JsonValue* deserialized_name = JsonValue_GetObjectProperty(deserialized_person, "name");
        JsonValue* deserialized_person_address = JsonValue_GetObjectProperty(deserialized_person, "address");
        JsonValue* deserialized_phones = JsonValue_GetObjectProperty(deserialized_person, "phones");
        
        printf("  ID: %d\n", deserialized_id ? deserialized_id->value.int_value : 0);
        printf("  Name: %s\n", deserialized_name ? deserialized_name->value.string_value : "NULL");
        
        if (deserialized_person_address) {
            JsonValue* addr_street = JsonValue_GetObjectProperty(deserialized_person_address, "street");
            JsonValue* addr_city = JsonValue_GetObjectProperty(deserialized_person_address, "city");
            printf("  Address: %s, %s\n", 
                   addr_street ? addr_street->value.string_value : "NULL",
                   addr_city ? addr_city->value.string_value : "NULL");
        }
        
        if (deserialized_phones) {
            printf("  Phones (%zu):\n", deserialized_phones->value.array_value.count);
            for (size_t i = 0; i < deserialized_phones->value.array_value.count; i++) {
                JsonValue* phone = &deserialized_phones->value.array_value.items[i];
                JsonValue* phone_number = JsonValue_GetObjectProperty(phone, "number");
                JsonValue* phone_type = JsonValue_GetObjectProperty(phone, "type");
                printf("    %s: %s\n", 
                       phone_type ? phone_type->value.string_value : "unknown",
                       phone_number ? phone_number->value.string_value : "NULL");
            }
        }
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    JsonValue_Free(address);
    JsonValue_Free(deserialized_address);
    JsonValue_Free(person);
    JsonValue_Free(deserialized_person);
    
    JsonSchema_Free(address_schema);
    JsonSchema_Free(person_schema);
    
    SerializationResult_Free(address_result);
    SerializationResult_Free(person_result);
    
    JsonSchemaSerializer_Cleanup();
    
    printf("Example completed successfully!\n");
    return 0;
}