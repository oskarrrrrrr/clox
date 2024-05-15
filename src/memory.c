#include "vm.h"
#include "common.h"
#include "object.h"
#include "memory.h"

#include <stdio.h>

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, newSize);
    if (result == NULL) {
        printf("Failed to realloc. oldSize: %ld, newSize: %ld\n", oldSize, newSize);
        exit(1);
    }
    return result;
}

static void freeObject(Obj* obj) {
    switch (obj->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)obj;
            size_t size = sizeof(ObjString) + sizeof(char) * (string->length+1);
            reallocate(string, size, 0);
            break;
        }
    }
}

void freeObjects() {
    Obj* obj = vm.objects;
    while (obj != NULL) {
        Obj* next = obj->next;
        freeObject(obj);
        obj = next;
    }
}
