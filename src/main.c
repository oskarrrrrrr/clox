#include "chunk.h"
#include "debug.h"
#include <stdio.h>

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    for (int i = 0; i < 1000; i++) {
        writeConstant(&chunk, 1.2, i);
    }
    writeChunk(&chunk, OP_RETURN, 1);
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}
