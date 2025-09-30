#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Minimal test without the full library
int main() {
    printf("Simple JSON Test\n");
    printf("================\n");
    
    const char* test_json = "{\"type\": \"string\"}";
    printf("Testing JSON: %s\n", test_json);
    
    // Simple character-by-character analysis
    for (int i = 0; test_json[i] != '\0'; i++) {
        printf("Char %d: '%c' (ASCII %d)\n", i, test_json[i], (int)test_json[i]);
    }
    
    return 0;
}