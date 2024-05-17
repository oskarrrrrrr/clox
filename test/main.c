/*
* Tests of the hash table implementation.
*/

#include "value.h"
#include "table.h"
#include "object.h"
#include "test.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* expectedGotMsg(Value expected, Value got) {
    char* msg = malloc(sizeof(char) * 1000);
    sprintf(msg, "Expected ");
    sprintValue(msg+strlen(msg), expected);
    sprintf(msg+strlen(msg), ", got: ");
    sprintValue(msg+strlen(msg), got);
    sprintf(msg+strlen(msg), ".");
    return msg;
}

static void randStr(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

static TestResult testTableGetFromEmpty() {
    TEST_NAME("TestTableGetFromEmpty");

    Table table;
    initTable(&table);

    ObjString* key = copyString("abc", 3);
    Value result;
    bool elementFound = tableGet(&table, key, &result);

    freeTable(&table);

    if (!elementFound) OK;
    FAIL("Expected no element to be found.");
}

static TestResult testTableSetAndGet() {
    TEST_NAME("TestTableSetAndGet");

    Table table;
    initTable(&table);

    ObjString* key = copyString("abc", 3);
    Value expected = NUMBER_VAL(123);

    tableSet(&table, key, expected);

    Value result;
    tableGet(&table, key, &result);

    freeTable(&table);

    if (valuesEqual(expected, result)) OK;
    FAIL(expectedGotMsg(expected, result));
}

static TestResult testTableInsertsAndDeletes() {
    TEST_NAME("TestTableInsertsAndDeletes");

    Table table;
    initTable(&table);

    #define N 100
    #define KEY_LEN 6
    char* strings[N];
    ObjString* keys[N];
    Value values[N];

    for (int i = 0; i < N; i++) {
        strings[i] = malloc(sizeof(char) * KEY_LEN + 1);
        randStr(strings[i], KEY_LEN);
        keys[i] = copyString(strings[i], KEY_LEN);
        values[i] = NUMBER_VAL(rand());
        tableSet(&table, keys[i], values[i]);
    }

    bool ok = true;
    char* errMsg;

    Value result;
    for (int i = 0; i < N && ok; i++) {
        bool found = tableGet(&table, keys[i], &result);
        if (!found || !valuesEqual(result, values[i])) {
            ok = false;
            errMsg = "One of the values is incorrect.";
            goto CleanUp;
        }
    }

    for (int i = 0; i < N && ok; i++) {
        bool deleted = tableDelete(&table, keys[i]);
        if (!deleted) {
            ok = false;
            errMsg = "Failed to delete one of the keys";
            goto CleanUp;
        }
    }

CleanUp:
    freeTable(&table);
    #undef N
    #undef KEY_LEN

    if (ok) OK;
    FAIL(errMsg);
}

TEST_FUN TESTS[] = {
    testTableGetFromEmpty,
    testTableSetAndGet,
    testTableInsertsAndDeletes,
};

int TESTS_COUNT = sizeof(TESTS) / sizeof(TEST_FUN);

int main() {
    int ok = 0;
    for (int i = 0; i < TESTS_COUNT; i++) {
        TestResult result = TESTS[i]();
        if (result.ok) ok++;
        printResult(result);
    }
    printf("[%d/%d]\n", ok, TESTS_COUNT);
    if (ok != TESTS_COUNT) {
        exit(20);
    }
    return 0;
}
