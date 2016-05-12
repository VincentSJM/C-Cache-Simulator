#include "trace.h"

// Put anything for reading trace files / writing output files here.
trace_t * Trace = NULL;

int getTraceLineNum(char* filePath) {

	int status;
	FILE *fp;
	char *source;
	int lines;

	fp = fopen(filePath, "rb");
	if (!fp) {
		printf("Could not open trace file\n");
		exit(-1);
	}

	status = fseek(fp, 0, SEEK_END);
	if (status != 0) {
		printf("Error seeking to end of file\n");
		exit(-1);
	}
	//s_0x1fffff50\n 13 characters
	lines = ftell(fp) / 13;

	rewind(fp);
	fclose(fp);
	return lines;
}

void initTraceArray(int traceLineNum) {
	Trace = (trace_t *) malloc(traceLineNum * sizeof(trace_t));
	if (!Trace) {
		printf("Could not malloc Trace array");
		exit(-1);
	}
	return;
}

void destroyTraceArray() {
	free(Trace);
	return;
}

/* TODO: read trace file function */
void readTraceFile(char* filePath, int trace_file_lines) {
	FILE *fp;
	fp = fopen(filePath, "rb");

	if (!fp) {
		printf("Could not open trace file\n");
		exit(-1);
	}

	int i;
	for (i = 0; i < trace_file_lines; i++) {
		fscanf(fp, "%c ", &(Trace[i].access));
		fscanf(fp, "%x\n", &(Trace[i].address));
		Trace[i].miss = compulsory; //?
	}
	fclose(fp);
	return;
}

/* TODO: write output file function */
void writeTraceFile(char* filePath, int trace_file_lines) {
	FILE *fp;
	fp = fopen(filePath, "wb");

	if (!fp) {
		printf("Could not open trace file\n");
		exit(-1);
	}

	int i;
	for (i = 0; i < trace_file_lines; i++) {
		fprintf(fp, "%c ", Trace[i].access);
		fprintf(fp, "0x%.8x ", Trace[i].address);
		switch (Trace[i].miss) {
		case hit:
			fprintf(fp, "hit\n");
			break;
		case compulsory:
			fprintf(fp, "compulsory\n");
			break;
		case conflict:
			fprintf(fp, "conflict\n");
			break;
		case capacity:
			fprintf(fp, "capacity\n");
			break;
		}
	}
	fclose(fp);
	return;
}
