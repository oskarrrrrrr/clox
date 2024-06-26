#include "value.h"
#include "memory.h"
#include "object.h"

#include <stdio.h>
#include <string.h>

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_NIL: return true;
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
        default: return false;
    }
}

void initValueArray(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count+1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(8, oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ: printObject(value); break;
    }
}

void sprintValue(char* buffer, Value value) {
    switch (value.type) {
        case VAL_BOOL: sprintf(buffer, AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL: sprintf(buffer, "nil"); break;
        case VAL_NUMBER: sprintf(buffer, "%g", AS_NUMBER(value)); break;
        case VAL_OBJ: sprintObject(buffer, value); break;
    }
}
