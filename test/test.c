#include "test.h"

#include <stdio.h>

void printResult(TestResult result) {
    printf("[%s] ", result.name);
    if (result.ok) {
        printf("OK");
    } else {
        printf("FAILED: %s", result.message);
    }
    printf("\n");
}
