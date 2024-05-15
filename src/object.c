#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

#define ALLOCATE_OBJ(type, objectType) (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* obj = (Obj*)reallocate(NULL, 0, size);
    obj->type = type;
    obj->next = vm.objects;
    vm.objects = obj;
    return obj;
}

static ObjString* allocateString(const char* chars, int length) {
    size_t size = sizeof(ObjString) + sizeof(char) * (length + 1);
    ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
    string->length = length;
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    return string;
}

ObjString* takeString(char* chars, int length) {
    return allocateString(chars, length);
}

ObjString* copyString(const char* chars, int length) {
    return allocateString(chars, length);
}

ObjString* concatenateStrings(ObjString* a, ObjString* b) {
    int length = a->length + b->length;
    size_t size = sizeof(ObjString) + sizeof(char) * (length + 1);
    ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
    string->length = length;
    memcpy(string->chars, a->chars, a->length);
    memcpy(string->chars + a->length, b->chars, b->length);
    string->chars[length] = '\0';
    return string;
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
