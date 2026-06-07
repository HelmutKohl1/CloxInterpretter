#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
	OP_CONSTANT,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NEGATE,
	OP_CONSTANT_LONG,
	OP_RETURN,
} OpCode;

typedef struct {
	int count;
	int capacity;
	int* lines;
	int* lineCounts;
} LineInfo;

typedef struct {
	int count;
	int capacity;
	uint8_t* code;
	LineInfo lineInfo;
	ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void writeConstant(Chunk* chunk, Value value, int line);
int addConstant(Chunk* chunk, Value value);
void freeChunk(Chunk* chunk);


#endif