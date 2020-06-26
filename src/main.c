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

		char32_t *line;
		while (true) {
			line=repl_read(stdin);

			const char32_t *tmp=repl_eval(line, ctx);
			char *output=c32stombs(tmp);
			if (output!=NULL) {
				printf("%s\n", output);
			}

			free(output);
			free(line);
			if (!is_error_msg(tmp)) {
				free((char32_t*)tmp);
			}
		}
	}

	else if (argc>2) {
		puts("no input files specified, exiting");
		return 1;
	}

	FILE *f=fopen(argv[1], "re");
	if (f==NULL) {
		printf("\"%s\" was not found, exiting", argv[1]);
		return 1;
	}

	eval_file(f);
	return 0;
}