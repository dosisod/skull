#include <stdio.h>

#include "../src/eval.h"

int main(int argc, char *argv[]) {
	if (argc!=2) {
		printf("no input files specified, exiting\n");
		return 1;
	}

	FILE *f;
	f=fopen(argv[1], "r");

	if (f==NULL) {
		printf("\"%s\" was not found, exiting\n", argv[1]);
		return 1;
	}

	eval(f);
	fclose(f);

	return 0;
}