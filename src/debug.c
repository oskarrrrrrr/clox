#include "debug.h"
#include "value.h"

#include <stdio.h>

void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, const Chunk* chunk, int offset) {
    u8 constant = chunk->code[offset+1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int longConstantInstruction(const char* name, const Chunk* chunk, int offset) {
    int constantIdx = chunk->code[offset+1] << 16;
    constantIdx += chunk->code[offset+2] << 8;
    constantIdx += chunk->code[offset+3];

    printf("%-16s %4d '", name, constantIdx);
    printValue(chunk->constants.values[constantIdx]);
    printf("'\n");
    return offset + 4;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);
    int currLine = getLine(chunk, offset);
    int prevLine = offset <= 0 ? -1 : getLine(chunk, offset-1);
    if (offset > 0 && currLine == prevLine) {
        printf("   | ");
    } else {
        printf("%4d ", getLine(chunk, offset));
    }

    u8 instruction = chunk->code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
        return longConstantInstruction("OP_CONSTANT_LONG", chunk, offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
