#include "debug.h"
#include "value.h"
#include "scanner.h"

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
    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);
    case OP_POP:
        return simpleInstruction("OP_POP", offset);
    case OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case OP_GET_GLOBAL_LONG:
        return longConstantInstruction("OP_GET_GLOBAL_LONG", chunk, offset);
    case OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case OP_SET_GLOBAL_LONG:
        return longConstantInstruction("OP_SET_GLOBAL_LONG", chunk, offset);
    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_DEFINE_GLOBAL_LONG:
        return longConstantInstruction("OP_DEFINE_GLOBAL_LONG", chunk, offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}

static const char* TOKEN_NAMES[] = {
    [TOKEN_LEFT_PAREN] = "TOKEN_LEFT_PAREN",
    [TOKEN_RIGHT_PAREN] = "TOKEN_RIGHT_PAREN",
    [TOKEN_LEFT_BRACE] = "TOKEN_LEFT_BRACE",
    [TOKEN_RIGHT_BRACE] = "TOKEN_RIGHT_BRACE",
    [TOKEN_COMMA] = "TOKEN_COMMA",
    [TOKEN_DOT] = "TOKEN_DOT",
    [TOKEN_MINUS] = "TOKEN_MINUS",
    [TOKEN_PLUS] = "TOKEN_PLUS",
    [TOKEN_SEMICOLON] = "TOKEN_SEMICOLON",
    [TOKEN_SLASH] = "TOKEN_SLASH",
    [TOKEN_STAR] = "TOKEN_STAR",
    [TOKEN_BANG] = "TOKEN_BANG",
    [TOKEN_BANG_EQUAL] = "TOKEN_BANG_EQUAL",
    [TOKEN_EQUAL] = "TOKEN_EQUAL",
    [TOKEN_EQUAL_EQUAL] = "TOKEN_EQUAL_EQUAL",
    [TOKEN_GREATER] = "TOKEN_GREATER",
    [TOKEN_GREATER_EQUAL] = "TOKEN_GREATER_EQUAL",
    [TOKEN_LESS] = "TOKEN_LESS",
    [TOKEN_LESS_EQUAL] = "TOKEN_LESS_EQUAL",
    [TOKEN_IDENTIFIER] = "TOKEN_IDENTIFIER",
    [TOKEN_STRING] = "TOKEN_STRING",
    [TOKEN_NUMBER] = "TOKEN_NUMBER",
    [TOKEN_AND] = "TOKEN_AND",
    [TOKEN_CLASS] = "TOKEN_CLASS",
    [TOKEN_ELSE] = "TOKEN_ELSE",
    [TOKEN_FALSE] = "TOKEN_FALSE",
    [TOKEN_FOR] = "TOKEN_FOR",
    [TOKEN_FUN] = "TOKEN_FUN",
    [TOKEN_IF] = "TOKEN_IF",
    [TOKEN_NIL] = "TOKEN_NIL",
    [TOKEN_OR] = "TOKEN_OR",
    [TOKEN_PRINT] = "TOKEN_PRINT",
    [TOKEN_RETURN] = "TOKEN_RETURN",
    [TOKEN_SUPER] = "TOKEN_SUPER",
    [TOKEN_THIS] = "TOKEN_THIS",
    [TOKEN_TRUE] = "TOKEN_TRUE",
    [TOKEN_VAR] = "TOKEN_VAR",
    [TOKEN_WHILE] = "TOKEN_WHILE",
    [TOKEN_ERROR] = "TOKEN_ERROR",
    [TOKEN_EOF] = "TOKEN_EOF",
};

const char* tokenName(TokenType type) {
    return TOKEN_NAMES[type];
}
