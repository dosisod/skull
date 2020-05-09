#include <locale.h>
#include <stdio.h>

#include "../src/eval.h"

/*
Run a `.skull` file passed in from the command line.

If the file was not found, or a file is not specified, skull sets an exit code of 1.

Else, an exit code of 0 is set.
*/
int main(int argc, char *argv[]) {
	if (!setlocale(LC_CTYPE, "")) {
		printf("Could not set locale.");
		return 1;
	}

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