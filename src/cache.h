#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

extern int write_xactions;
extern int read_xactions;

#define dirty true
#define clean false

#define valid true
#define invalid false

void printHelp(const char * prog);

typedef struct {
	bool valid_bit;bool dirty_bit;
	uint32_t tag;
	uint32_t priority;
} cache_t;

typedef enum {
	FIFO, LRU
} policy_t;

extern cache_t ** Cache;
extern cache_t * fullyAssociativeCache;

void initCache(uint32_t size, uint32_t ways, uint32_t line) {
	uint32_t sets = (size * 1024) / (line * ways);

	Cache = (cache_t **) malloc(sets * sizeof(cache_t *));

	if (!Cache) {
		printf("Could not malloc Cache\n");
		exit(-1);
	}

	int i, j;
	for (i = 0; i < sets; i++) {
		Cache[i] = (cache_t *) malloc(ways * sizeof(cache_t));
		if (!Cache[i]) {
			printf("Could not malloc Cache[%d]\n", i);
			exit(-1);
		}
	}

	for (i = 0; i < sets; i++) {
		for (j = 0; j < ways; j++) {
			Cache[i][j].valid_bit = invalid;
			Cache[i][j].dirty_bit = clean;
			Cache[i][j].tag = 0;
			Cache[i][j].priority = UINT32_MAX;
		}
	}

	return;
}

void destroyCache(uint32_t size, uint32_t ways, uint32_t line) {
	uint32_t sets = (size * 1024) / (line * ways);
	int i;
	for (i = sets - 1; i >= 0; i--) {
		free(Cache[i]);
	}
	free(Cache); //?
	return;
}

void initFullyAssociativeCache(uint32_t size, uint32_t ways, uint32_t line) {
	uint32_t slots = (size * 1024) / line;
	fullyAssociativeCache = (cache_t *) malloc(slots * sizeof(cache_t)); //Fixed
	if (!fullyAssociativeCache) {
		printf("Could not malloc fully associative Cache\n");
		exit(-1);
	}
	int i;
	for (i = 0; i < slots; i++) {
		fullyAssociativeCache[i].valid_bit = invalid;
		fullyAssociativeCache[i].dirty_bit = clean;
		fullyAssociativeCache[i].tag = 0;
		fullyAssociativeCache[i].priority = UINT32_MAX;
	}
	return;
}

void destroyFullyAssociativeCache() {
	free(fullyAssociativeCache);
	return;
}
