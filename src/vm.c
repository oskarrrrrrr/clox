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
    initTable(&vm.globals);
    initTable(&vm.strings);
}

void freeVM() {
    freeTable(&vm.globals);
    freeTable(&vm.strings);
    freeObjects();
}


static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG() (vm.chunk->constants.values[(READ_BYTE() << 16) + (READ_BYTE() << 8) + READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_STRING_LONG() AS_STRING(READ_CONSTANT_LONG())
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
#define GET_GLOBAL(name) \
    do { \
        Value value; \
        if (!tableGet(&vm.globals, name, &value)) { \
            runtimeError("Undefined variable '%s'.", name->chars); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        push(value); \
    } while(false)
#define SET_GLOBAL(name) \
    do { \
        if (tableSet(&vm.globals, name, peek(0))) { \
            tableDelete(&vm.globals, name); \
            runtimeError("Undefined variable '$s'.", name->chars); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
    } while(false)
#define DEFINE_GLOBAL(name) \
    do { \
        tableSet(&vm.globals, name, peek(0)); \
        pop(); \
    } while(false)


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
                push(READ_CONSTANT());
                break;
            }
            case OP_CONSTANT_LONG: {
                push(READ_CONSTANT_LONG());
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
            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;
            }
            case OP_POP: pop(); break;
            case OP_GET_GLOBAL: GET_GLOBAL(READ_STRING()); break;
            case OP_GET_GLOBAL_LONG: GET_GLOBAL(READ_STRING_LONG()); break;
            case OP_SET_GLOBAL: SET_GLOBAL(READ_STRING()); break;
            case OP_SET_GLOBAL_LONG: SET_GLOBAL(READ_STRING_LONG()); break;
            case OP_DEFINE_GLOBAL: DEFINE_GLOBAL(READ_STRING()); break;
            case OP_DEFINE_GLOBAL_LONG: DEFINE_GLOBAL(READ_STRING_LONG()); break;
            case OP_RETURN: return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
#undef READ_STRING
#undef READ_STRING_LONG
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
