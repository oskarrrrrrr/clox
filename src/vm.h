#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "table.h"

typedef struct {
    Chunk* chunk;
    u8* ip;

    Value* stack;
    Value* stackTop;
    int stackCapacity;

    Table globals;
    Table strings;
    Obj* objects;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif
