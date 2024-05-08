#include "chunk.h"
#include "memory.h"
#include "value.h"
#include <stdlib.h>
#include <stdio.h>

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    chunk->linesCount = 0;
    chunk->linesCapacity = 0;
    chunk->lines = NULL;

    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(u8, chunk->code, chunk->capacity);
    FREE_ARRAY(LineInfo, chunk->lines, chunk->linesCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, u8 byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(u8, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;

    if (chunk->linesCount == 0 || chunk->lines[chunk->linesCount-1].line != line) {
        if (chunk->linesCapacity < chunk->linesCount + 1) {
            int oldCapacity = chunk->linesCapacity;
            chunk->linesCapacity = GROW_CAPACITY(oldCapacity);
            chunk->lines = GROW_ARRAY(LineInfo, chunk->lines, oldCapacity, chunk->linesCapacity);
        }
        chunk->lines[chunk->linesCount].line = line;
        chunk->lines[chunk->linesCount].count = 1;
        chunk->linesCount++;
    } else {
        chunk->lines[chunk->linesCount-1].count++;
    }
}

static int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, Value value, int line) {
    if (chunk->constants.count < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        int constIdx = addConstant(chunk, value);
        writeChunk(chunk, constIdx, line);
    } else {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        int constIdx = addConstant(chunk, value);
        writeChunk(chunk, constIdx >> 16, line);
        writeChunk(chunk, constIdx >> 8, line);
        writeChunk(chunk, constIdx, line);
    }
}

int getLine(Chunk* chunk, int offset) {
    int count = 0;
    for (int i = 0; i < chunk->linesCount; i++) {
        count += chunk->lines[i].count;
        if (count > offset) {
            return chunk->lines[i].line;
        }
    }
    // TODO: Log error.
    return -1;
}
