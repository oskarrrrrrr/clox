#include "chunk.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    initVM();

    /* writeConstant(&chunk, 2, 1); */
    /* for (int i = 0; i < 100000000; i++) { */
    /*     writeChunk(&chunk, OP_NEGATE, 1); */
    /* } */

    for (int i = 0; i < 600; i++) {
        writeConstant(&chunk, 2, 1);
    }

    /* writeConstant(&chunk, 3, 1); */
    /* writeChunk(&chunk, OP_MULTIPLY, 1); */
    /* writeConstant(&chunk, 1, 1); */
    /* writeChunk(&chunk, OP_ADD, 1); */
    /* writeConstant(&chunk, 4, 1); */
    /* writeConstant(&chunk, 5, 1); */
    /* writeChunk(&chunk, OP_NEGATE, 1); */
    /* writeChunk(&chunk, OP_DIVIDE, 1); */
    /* writeChunk(&chunk, OP_SUBTRACT, 1); */

    writeChunk(&chunk, OP_RETURN, 2);

    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);

    return 0;
}
