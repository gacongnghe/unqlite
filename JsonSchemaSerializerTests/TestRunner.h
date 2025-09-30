#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test framework macros
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s:%d - Assertion failed: %s\n", __FILE__, __LINE__, #condition); \
            return 0; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            printf("FAIL: %s:%d - Assertion failed: %s should be false\n", __FILE__, __LINE__, #condition); \
            return 0; \
        } \
    } while(0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("FAIL: %s:%d - Expected %d, got %d\n", __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return 0; \
        } \
    } while(0)

#define ASSERT_STR_EQ(expected, actual) \
    do { \
        if (strcmp(expected, actual) != 0) { \
            printf("FAIL: %s:%d - Expected '%s', got '%s'\n", __FILE__, __LINE__, expected, actual); \
            return 0; \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("FAIL: %s:%d - Expected non-null pointer\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while(0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("FAIL: %s:%d - Expected null pointer\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while(0)

// Test function type
typedef int (*TestFunction)(void);

// Test runner structure
typedef struct {
    const char* name;
    TestFunction function;
} TestCase;

// Test runner functions
int RunTests(const TestCase* tests, int test_count);
void PrintTestResults(int passed, int total);

// Individual test modules
int TestJsonSchemaParser(void);
int TestBinarySerializer(void);
int TestJsonSchemaValidator(void);
int TestJsonSchemaSerializer(void);