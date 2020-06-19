#include <locale.h>
#include <stdio.h>
#include <wchar.h>

#include "../src/eval/repl.h"
#include "../src/eval.h"

/*
If no parameters are passed, run in interactive (repl) mode.

Else, run a passed in `.skull` file.
*/
int main(int argc, char *argv[]) {
	if (!setlocale(LC_CTYPE, "")) {
		puts("Could not set locale.\n");
		return 1;
	}

	if (argc==1) {
		puts("Skull " SKULL_VERSION);

		context_t *ctx=make_context();
		make_default_types();

		wchar_t *line;
		while (true) {
			line=repl_read();

			const wchar_t *output=repl_eval(line, ctx);
			if (output!=NULL) {
				printf("%ls\n", output);
			}

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

	eval(f);
	fclose(f);
	return 0;
}