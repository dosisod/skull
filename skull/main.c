#include <stdio.h>

#include "skull/errors.h"
#include "skull/common/local.h"
#include "skull/common/main.h"
#include "skull/eval/file.h"
#include "skull/eval/repl.h"

/*
If no parameters are passed, run in interactive (repl) mode.

Else, run a passed in `.sk` file.
*/
int main(int argc, char *argv[]) {
	SETUP_LOCALE();

	if (argc == 1) {
		puts("Skull " SKULL_VERSION);
		context_t *ctx = make_context();

		while (true) {
			repl_loop(stdin, ctx);
		}
	}

	HANDLE_MAIN;

	eval_file(f);
	return 0;
}