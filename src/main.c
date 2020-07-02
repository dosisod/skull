#include <stdio.h>
#include <stdlib.h>

#include "../src/errors.h"
#include "../src/common/local.h"
#include "../src/eval/file.h"
#include "../src/eval/repl.h"

#ifndef SKULL_VERSION
#define SKULL_VERSION "<version unknown>"
#endif

/*
If no parameters are passed, run in interactive (repl) mode.

Else, run a passed in `.skull` file.
*/
int main(int argc, char *argv[]) {
	SETUP_LOCALE();

	if (argc==1) {
		puts("Skull " SKULL_VERSION);
		context_t *ctx=make_context();

		while (true) {
			repl_loop(stdin, ctx);
		}
	}

	else if (argc > 2) {
		puts("too many input files specified, exiting");
		return 1;
	}

	if (strcmp("-v", argv[1])==0) {
		puts("Skull " SKULL_VERSION);
		return 0;
	}

	if (strrstr(argv[1], ".skull")==NULL) {
		puts("missing required \".skull\" extension, exiting");
		return 1;
	}

	FILE *f=fopen(argv[1], "re");
	if (f==NULL) {
		printf("\"%s\" was not found, exiting\n", argv[1]);
		return 1;
	}

	eval_file(f);
	return 0;
}