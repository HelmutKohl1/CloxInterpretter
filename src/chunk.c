#include <stdlib.h>

#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->lineInfo.count = 0;
	chunk->lineInfo.capacity = 0;
	chunk->lineInfo.lines = NULL;
	chunk->lineInfo.lineCounts = NULL;
	chunk->code = NULL;
	initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
	}
	
	chunk->code[chunk->count] = byte;

	unsigned int lastLineIdx = chunk->lineInfo.count;
	if (lastLineIdx > 0 && chunk->lineInfo.lines[lastLineIdx] == line) {
		//found, so increment corresponding count
		chunk->lineInfo.lineCounts[lastLineIdx]++; 
	} else {
		// add new line and line count, check if array needs to be resized
		if (chunk->lineInfo.capacity < lastLineIdx + 1) {
			unsigned int oldCapacity = chunk->lineInfo.capacity;
			chunk->lineInfo.capacity = GROW_CAPACITY(chunk->lineInfo.capacity);
			chunk->lineInfo.lines = GROW_ARRAY(int, chunk->lineInfo.lines, oldCapacity, chunk->lineInfo.capacity);
			chunk->lineInfo.lineCounts = GROW_ARRAY(int, chunk->lineInfo.lineCounts, oldCapacity, chunk->lineInfo.capacity);
		}
		chunk->lineInfo.lines[lastLineIdx] = line;
		chunk->lineInfo.lineCounts[lastLineIdx] = 1;
		chunk->lineInfo.count++;
	}
	
	chunk->count++;
}

void writeConstant(Chunk* chunk, Value value, int line) {	
	int constant = addConstant(chunk, value);
	if (chunk->constants.count < 256) {
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, (uint8_t)constant, line);
	} else {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunk(chunk, (uint8_t)(constant & 0xFF), line); // write first 8 bytes
		writeChunk(chunk, (uint8_t)((constant >> 8) & 0xFF), line); // write second 8 bytes
		writeChunk(chunk, (uint8_t)((constant >> 16) & 0xFF), line); // write last 8 bytes
	}	
}

int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1; // this is the index where the constant was appended
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lineInfo.lines, chunk->lineInfo.capacity);
	FREE_ARRAY(int, chunk->lineInfo.lineCounts, chunk->lineInfo.capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}

