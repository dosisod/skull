#include <stdio.h>

#include "../src/common/local.h"
#include "../src/eval/repl.h"
#include "../src/eval_file.h"

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
		}
	}

	else if (argc>2) {
		puts("no input files specified, exiting");
		return 1;
	}

	FILE *f=fopen(argv[1], "r");
	if (f==NULL) {
		printf("\"%s\" was not found, exiting", argv[1]);
		return 1;
	}

	eval_file(f);
	return 0;
}