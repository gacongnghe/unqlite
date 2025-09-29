#include "TestRunner.h"
#include "TestJsonSchemaParser.h"
#include "TestBinarySerializer.h"
#include "TestJsonSchemaValidator.h"

// Forward declaration for the missing test
int TestJsonSchemaSerializer(void);

int main() {
    printf("JsonSchemaSerializer Unit Tests\n");
    printf("================================\n\n");
    
    TestCase tests[] = {
        {"JsonSchemaParser", TestJsonSchemaParser},
        {"BinarySerializer", TestBinarySerializer},
        {"JsonSchemaValidator", TestJsonSchemaValidator},
        {"JsonSchemaSerializer", TestJsonSchemaSerializer}
    };
    
    int test_count = sizeof(tests) / sizeof(tests[0]);
    int passed = RunTests(tests, test_count);
    
    PrintTestResults(passed, test_count);
    
    return (passed == test_count) ? 0 : 1;
}

int RunTests(const TestCase* tests, int test_count) {
    int passed = 0;
    
    for (int i = 0; i < test_count; i++) {
        printf("Running %s... ", tests[i].name);
        fflush(stdout);
        
        if (tests[i].function()) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
        }
    }
    
    return passed;
}

void PrintTestResults(int passed, int total) {
    printf("\n================================\n");
    printf("Test Results: %d/%d passed\n", passed, total);
    
    if (passed == total) {
        printf("All tests passed! ✓\n");
    } else {
        printf("Some tests failed! ✗\n");
    }
}