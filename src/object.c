#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "table.h"

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

static size_t objSize(Obj* obj) {
    switch (obj->type) {
        case OBJ_STRING: {
            ObjString* objStr = (ObjString*)obj;
            return sizeof(ObjString) + (sizeof(char) * (objStr->length + 1));
        }
    }
}

static void freeTopObject() {
    Obj* toFree = vm.objects;
    vm.objects = vm.objects->next;
    size_t size = objSize(toFree);
    reallocate(toFree, size, 0);
}

static u32 hashString(const char* key, int length) {
    u32 hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (u8)key[i];
        hash *= 16777619;
    }
    return hash;
}

static ObjString* allocateString(const char* chars, int length) {
    u32 hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) return interned;

    size_t size = sizeof(ObjString) + sizeof(char) * (length + 1);
    ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
    string->length = length;
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
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
    string->hash = hashString(string->chars, length);
    ObjString* interned = tableFindString(
        &vm.strings, string->chars, string->length, string->hash
    );
    if (interned != NULL) {
        freeTopObject();
        return interned;
    }
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
