#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,
} OpCode;

typedef struct {
    int line;
    int count;
} LineInfo;

typedef struct {
    int count;
    int capacity;
    u8* code;

    int linesCount;
    int linesCapacity;
    LineInfo* lines;

    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, u8 byte, int line);
void writeConstant(Chunk* chunk, Value value, int line);
int getLine(Chunk* chunk, int offset);

#endif
