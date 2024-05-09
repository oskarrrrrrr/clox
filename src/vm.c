#include "vm.h"
#include "debug.h"
#include "memory.h"

#include <stdio.h>

VM vm;

static void resetStack() {
    int START_STACK_CAPACITY = 256;
    vm.stack = GROW_ARRAY(Value, vm.stack, vm.stackCapacity, START_STACK_CAPACITY);
    vm.stackTop = vm.stack;
    vm.stackCapacity = START_STACK_CAPACITY;
}

void initVM() {
    resetStack();
}

void freeVM() {
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(op) \
    do { \
        Value b = pop(); \
        Value a = pop(); \
        push(a op b); \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot != vm.stackTop; slot++) {
            printf("[");
            printValue(*slot);
            printf("]");
        }
        printf("\n");

        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        u8 instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = vm.chunk->constants.values[READ_BYTE()];
                push(constant);
                break;
            }
            case OP_CONSTANT_LONG: {
                int constIdx = (READ_BYTE() << 16) + (READ_BYTE() << 8) + READ_BYTE();
                Value constant = vm.chunk->constants.values[constIdx];
                push(constant);
                break;
            }
            case OP_NEGATE: *(vm.stackTop-1) *= -1; break;
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_RETURN:
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void push(Value value) {
    if (vm.stackTop - vm.stack >= vm.stackCapacity) {
        int idx = vm.stackTop - vm.stack;
        int oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(256, vm.stackCapacity);
        vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
        vm.stackTop = vm.stack + idx;
    }
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}
