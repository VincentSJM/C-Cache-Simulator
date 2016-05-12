#include "cache.h"
#include "trace.h"

int write_xactions = 0;
int read_xactions = 0;

cache_t ** Cache = NULL;
cache_t * fullyAssociativeCache = NULL;

uint32_t tag_bits;
uint32_t index_bits;
uint32_t offset_bits;

/*
 Print help message to user
 */
void printHelp(const char * prog) {
	printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n",
			prog);
	printf("options:\n");
	printf("-h: print out help text\n");
	printf("-s <cache size>: set the total size of the cache in KB\n");
	printf("-w <ways>: set the number of ways in each set\n");
	printf("-l <line size>: set the size of each cache line in bytes\n");
	printf("-t <trace>: use <trace> as the input file for memory traces\n");
	/* EXTRA CREDIT
	 printf("-lru: use LRU replacement policy instead of FIFO\n");
	 */
}

bool isAppeared(int traceCounter) {
	uint32_t tag = Trace[traceCounter].address;
	tag >>= offset_bits;
	int i;
	for (i = 0; i < traceCounter; i++) {
		if (tag == (((uint32_t) Trace[i].address) >> offset_bits))
			return true;
	}
	return false;
}

miss_t simFullyAssociativeCache(int traceCounter, policy_t policy,
		uint32_t slots,
		bool appeared) {

	uint32_t fully_tag_bits = tag_bits + index_bits;
	uint32_t tag = Trace[traceCounter].address;
	tag >>= (32 - fully_tag_bits);

	int i;
	for (i = 0; i < slots; i++) { //Aging
		if (fullyAssociativeCache[i].valid_bit == valid) {
			fullyAssociativeCache[i].priority++;
		}
	}

	for (i = 0; i < slots; i++) { //Find Hit
		if (fullyAssociativeCache[i].valid_bit == valid
				&& fullyAssociativeCache[i].tag == tag) {
			if (policy == LRU)
				fullyAssociativeCache[i].priority = 0; //Highest

			switch (Trace[traceCounter].access) {
			case 'l':
				//fullyAssociativeCache[i].dirty_bit = clean;
				break;
			case 's':
				fullyAssociativeCache[i].dirty_bit = dirty;
			}
			return hit;
		}
	}

	for (i = 0; i < slots; i++) { //Find Empty Space
		if (fullyAssociativeCache[i].valid_bit == invalid) {
			fullyAssociativeCache[i].valid_bit = valid;
			fullyAssociativeCache[i].tag = tag;
			fullyAssociativeCache[i].priority = 0;

			switch (Trace[traceCounter].access) {
			case 'l':
				fullyAssociativeCache[i].dirty_bit = clean;
				break;
			case 's':
				fullyAssociativeCache[i].dirty_bit = dirty;
			}
			if (!appeared) {
				return compulsory;
			} else {
				printf(
						"line %d : %c %#x is Appeared, But it get a Empty Space in Fully\n",
						traceCounter, Trace[traceCounter].access,
						Trace[traceCounter].address);
				exit(-1);
			}
		}
	}

	uint32_t prio = 0;
	int idx;
	for (i = 0; i < slots; i++) { //Find the Oldest
		if (fullyAssociativeCache[i].valid_bit == valid) {
			if (fullyAssociativeCache[i].priority > prio) {
				prio = fullyAssociativeCache[i].priority;
				idx = i;
			}
		}
	}

	fullyAssociativeCache[idx].tag = tag;
	fullyAssociativeCache[idx].priority = 0;
	switch (Trace[traceCounter].access) {
	case 'l':
		fullyAssociativeCache[idx].dirty_bit = clean;
		break;
	case 's':
		fullyAssociativeCache[idx].dirty_bit = dirty;
	}

	if (!appeared) {
		return compulsory;
	} else {
		return capacity;
	}
}

void simCache(int traceCounter, policy_t policy, uint32_t ways, uint32_t sets,
bool appeared, miss_t simFullyMissType, int *totalHits, int *totalMisses) {

	uint32_t tag = Trace[traceCounter].address;
	tag >>= (32 - tag_bits);
	uint32_t index = Trace[traceCounter].address;
	index <<= tag_bits;
	index >>= (tag_bits + offset_bits);
	if (tag_bits + offset_bits == 32)
		index = 0;

	int i;
	for (i = 0; i < ways; i++) { //Aging
		if (Cache[index][i].valid_bit == valid) {
			Cache[index][i].priority++;
		}
	}

	for (i = 0; i < ways; i++) { //Find hit
		if (Cache[index][i].valid_bit == valid && Cache[index][i].tag == tag) {

			if (policy == LRU)
				Cache[index][i].priority = 0;

			switch (Trace[traceCounter].access) {
			case 'l':
				//Cache[index][i].dirty_bit = clean;
				break;
			case 's':
				Cache[index][i].dirty_bit = dirty;
			}

			Trace[traceCounter].miss = hit;
			*totalHits += 1;
			return;
		}
	}

	*totalMisses += 1;

	for (i = 0; i < ways; i++) { //Find Empty Space
		if (Cache[index][i].valid_bit == invalid) {
			read_xactions++;

			Cache[index][i].valid_bit = valid;
			Cache[index][i].tag = tag;
			Cache[index][i].priority = 0;

			switch (Trace[traceCounter].access) {
			case 'l':
				Cache[index][i].dirty_bit = clean;
				break;
			case 's':
				Cache[index][i].dirty_bit = dirty;
			}
			if (!appeared) {
				Trace[traceCounter].miss = compulsory;
				return;
			} else {
				printf(
						"line %d : %c %#x is Appeared, But it get a Empty Space\n",
						traceCounter, Trace[traceCounter].access,
						Trace[traceCounter].address);
				exit(-1);
			}
		}
	}

	uint32_t prio = 0;
	int idx = 0;
	for (i = 0; i < ways; i++) { //Find the Oldest
		if (Cache[index][i].valid_bit == valid) {
			if (Cache[index][i].priority > prio) {
				prio = Cache[index][i].priority;
				idx = i;
			}
		}
	}

	if (Cache[index][idx].dirty_bit == dirty)
		write_xactions++; //write back
	read_xactions++;

	Cache[index][idx].tag = tag;
	Cache[index][idx].priority = 0;

	switch (Trace[traceCounter].access) {
	case 'l':
		Cache[index][idx].dirty_bit = clean;
		break;
	case 's':
		Cache[index][idx].dirty_bit = dirty;
	}

	if (!appeared) {
		Trace[traceCounter].miss = compulsory;
		return;
	}

	if (simFullyMissType == capacity)
		Trace[traceCounter].miss = capacity;
	else
		Trace[traceCounter].miss = conflict;
	return;

}
/*
 Main function. Feed to options to set the cache

 Options:
 -h : print out help message
 -s : set L1 cache Size (KB)
 -w : set L1 cache ways
 -l : set L1 cache line size
 */
int main(int argc, char* argv[]) {
	int i;
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)
	policy_t policy = FIFO;

	// hit and miss counts
	int totalHits = 0;
	int totalMisses = 0;

	char * filename;

	//strings to compare
	const char helpString[] = "-h";
	const char sizeString[] = "-s";
	const char waysString[] = "-w";
	const char lineString[] = "-l";
	const char traceString[] = "-t";
	const char lruString[] = "-lru";

	if (argc == 1) {
		// No arguments passed, show help
		printHelp(argv[0]);
		return 1;
	}

	//parse command line
	for (i = 1; i < argc; i++) {
		//check for help
		if (!strcmp(helpString, argv[i])) {
			//print out help text and terminate
			printHelp(argv[0]);
			return 1; //return 1 for help termination
		}
		//check for size
		else if (!strcmp(sizeString, argv[i])) {
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if (isdigit(argv[i][0])) {
				size = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of size value\n");
				return -1; //input failure
			}
		}
		//check for ways
		else if (!strcmp(waysString, argv[i])) {
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if (isdigit(argv[i][0])) {
				ways = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of ways value\n");
				return -1; //input failure
			}
		}
		//check for line size
		else if (!strcmp(lineString, argv[i])) {
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if (isdigit(argv[i][0])) {
				line = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of line size value\n");
				return -1; //input failure
			}
		} else if (!strcmp(traceString, argv[i])) {
			filename = argv[++i];
		} else if (!strcmp(lruString, argv[i])) {
			// Extra Credit: Implement Me!
			//printf("Unrecognized argument. Exiting.\n");
			//return -1;
			policy = LRU;
		}
		//unrecognized input
		else {
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}

//	#include <stdio.h>
//
//	int main ()
//	{
//	  freopen ("myfile.txt","w",stdout);
//	  printf ("This sentence is redirected to a file.");
//	  fclose (stdout);
//	  return 0;
//	}
	char tracename[100];
	char tracestdoutfile[200];
	char traceGoldfile[200];
	strncpy(tracename, filename, strlen(filename) - 6);			//.trace
	tracename[strlen(filename) - 6] = '\0';
	if (policy == FIFO) {
		sprintf(tracestdoutfile, "%s_s%u_w%u_l%u.stdout.GOLD", tracename, size,
				ways, line);
		sprintf(traceGoldfile, "%s_s%u_w%u_l%u.GOLD", tracename, size, ways,
				line);
	} else if (policy == LRU) {
		sprintf(tracestdoutfile, "%s_s%u_w%u_l%u_LRU.stdout.GOLD", tracename,
				size, ways, line);
		sprintf(traceGoldfile, "%s_s%u_w%u_l%u_LRU.GOLD", tracename, size, ways,
				line);
	}

	freopen(tracestdoutfile, "w", stdout);
	/* TODO: Probably should intitalize the cache */
	uint32_t sets = (size * 1024) / (line * ways);
	uint32_t slots = (size * 1024) / line;

	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line/* FIXed */);

	for (i = 0; line >> i != 1; i++)
		;
	offset_bits = i;

	for (i = 0; sets >> i != 1; i++)
		;
	index_bits = i;

	tag_bits = 32 - index_bits - offset_bits;
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tag_bits,
			index_bits, offset_bits/* FIXed */);

	/* TODO: Now we read the trace file line by line */
	const int traceLineNum = getTraceLineNum(filename);
	initTraceArray(traceLineNum);
	readTraceFile(filename, traceLineNum);

	/* TODO: Now we simulate the cache */
	initCache(size, ways, line);
	initFullyAssociativeCache(size, ways, line);

	for (i = 0; i < traceLineNum; i++) {
		bool appeared = isAppeared(i);
		miss_t simFullyMissType = simFullyAssociativeCache(i, policy, slots,
				appeared);
		simCache(i, policy, ways, sets, appeared, simFullyMissType, &totalHits,
				&totalMisses);
	}

	/* Print results */
	printf("Miss Rate: %8lf%%\n",
			((double) totalMisses) / ((double) totalMisses + (double) totalHits)
					* 100.0);
	printf("Read Transactions: %d\n", read_xactions);
	printf("Write Transactions: %d\n", write_xactions);
	fclose(stdout);

	/* TODO: Now we output the file */

	writeTraceFile(traceGoldfile, traceLineNum);

	/* TODO: Cleanup */
	destroyFullyAssociativeCache();
	destroyCache(size, ways, line);
	destroyTraceArray();

	return 0;
}
