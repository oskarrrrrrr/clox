#ifndef clox_test_h
#define clox_test_h

#include "common.h"

typedef struct {
    bool ok;
    char* name;
    char* message;
} TestResult;

void printResult(TestResult result);

#define OK        return ((TestResult){ .ok = true,  .name = __TEST_NAME, .message = NULL })
#define FAIL(msg) return ((TestResult){ .ok = false, .name = __TEST_NAME, .message = (msg) })

#define TEST_NAME(name) char* __TEST_NAME = name
typedef TestResult (*TEST_FUN)();

#endif
