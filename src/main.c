#include <locale.h>
#include <wchar.h>

#include "../src/eval/repl.h"
#include "../src/eval.h"

/*
If `-i` option is set, run in interactive (repl) mode.

Else, run the passed `.skull` file.
*/
int main(int argc, char *argv[]) {
	if (!setlocale(LC_CTYPE, "")) {
		wprintf(L"Could not set locale.");
		return 1;
	}
	if (argc!=2) {
		wprintf(L"no input files specified, exiting\n");
		return 1;
	}

	if (strcmp(argv[1], "-i")==0) {
		context_t *ctx=make_context();
		make_default_types();

		wchar_t *line;
		while (true) {
			line=repl_read();

			const wchar_t *output=repl_eval(line, ctx);
			if (output!=NULL) {
				wprintf(L"%ls\n", output);
			}

			free(line);
		}
	}
	else {
		FILE *f;
		f=fopen(argv[1], "r");

		if (f==NULL) {
			wprintf(L"\"%s\" was not found, exiting\n", argv[1]);
			return 1;
		}

		eval(f);
		fclose(f);
	}

	return 0;
}