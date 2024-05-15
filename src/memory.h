#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

#define ALLOCATE(type, count) ((type*)reallocate(NULL, 0, sizeof(type)*count))
#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0)

#define GROW_CAPACITY(defaultCapacity, capacity) \
    ((capacity) < defaultCapacity ? defaultCapacity : (capacity) * 2)

#define FREE_ARRAY(type, pointer, count) \
    reallocate(pointer, sizeof(type) * count, 0)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * oldCount, sizeof(type) * newCount)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);
void freeObjects();

#endif
