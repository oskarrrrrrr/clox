#include "vm.h"
#include "debug.h"
#include "memory.h"
#include "compiler.h"
#include "value.h"
#include "object.h"


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

VM vm;

static Value peek(int distance);
static bool isFalsey(Value value);
static void concatenate();

static void resetStack() {
    int START_STACK_CAPACITY = 256;
    vm.stack = GROW_ARRAY(Value, vm.stack, vm.stackCapacity, START_STACK_CAPACITY);
    vm.stackTop = vm.stack;
    vm.stackCapacity = START_STACK_CAPACITY;
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = getLine(vm.chunk, instruction);
    fprintf(stderr, "[line: %d] in script\n", line);
    resetStack();
}

void initVM() {
    resetStack();
    vm.objects = NULL;
}

void freeVM() {
    freeObjects();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        f64 b = AS_NUMBER(pop()); \
        f64 a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
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
            case OP_NIL: push(NIL_VAL); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                AS_NUMBER(*(vm.stackTop-1)) *= -1;
                break;
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER: BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS: BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD: {
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    BINARY_OP(NUMBER_VAL, +); break;
                } else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE: BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT: push(BOOL_VAL(isFalsey(pop()))); break;
            case OP_RETURN:
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
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

static Value peek(int distance) {
    return vm.stackTop[-1-distance];
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());
    ObjString* result = concatenateStrings(a, b);
    push(OBJ_VAL(result));
}
