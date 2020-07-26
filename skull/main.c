#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "common/local.h"
#include "eval/file.h"
#include "eval/repl.h"

#ifndef SKULL_VERSION
#define SKULL_VERSION "<version unknown>"
#endif

/*
If no parameters are passed, run in interactive (repl) mode.

Else, run a passed in `.sk` file.
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
		puts("too many arguments passed, exiting");
		return 1;
	}

	if (strcmp("-v", argv[1])==0) {
		puts("Skull " SKULL_VERSION);
		return 0;
	}

	if (strrstr(argv[1], ".sk")==NULL) {
		puts("missing required \".sk\" extension, exiting");
		return 1;
	}

	if (strrstr(argv[1], "/.sk")!=NULL) {
		puts("\".sk\" is not a valid name, exiting");
		return 1;
	}

	errno=0;
	FILE *f=fopen(argv[1], "re");
	if (f==NULL) {
		if (errno==EACCES) {
			printf("cannot open \"%s\", permission denied\n", argv[1]);
		}
		else if (errno==ENOENT) {
			printf("\"%s\" was not found, exiting\n", argv[1]);
		}
		return 1;
	}

	eval_file(f);
	return 0;
}