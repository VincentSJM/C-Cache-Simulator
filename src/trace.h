#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

typedef enum {
	hit, compulsory, conflict, capacity
} miss_t;

typedef struct {
	char access;
	uint32_t address;
	miss_t miss;
} trace_t;

extern trace_t * Trace;

int getTraceLineNum(char* filePath);

void initTraceArray(int traceLineNum);

void destroyTraceArray();

void readTraceFile(char* filePath, int trace_file_lines);

void writeTraceFile(char* filePath, int trace_file_lines);

