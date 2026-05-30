#include <stdlib.h>

#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	//chunk->lines = NULL;
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
		//chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);//
	}
	
	chunk->code[chunk->count] = byte;
	/*
	if line == last recorded line (needs to go sequentially!) then corresponding linecount++;
	else add line as new line, add new count init to 1
	*/
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
	
	//chunk->lines[chunk->count] = line; //<= old way
	chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1; // this is the index where the constant was appended
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	//FREE_ARRAY(int, chunk->lines, chunk->capacity);
	FREE_ARRAY(int, chunk->lineInfo.lines, chunk->lineInfo.capacity);
	FREE_ARRAY(int, chunk->lineInfo.lineCounts, chunk->lineInfo.capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}
