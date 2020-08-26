#include <stdlib.h>

#include "skull/errors.h"
#include "skull/eval/context.h"
#include "skull/eval/repl.h"

#include "skull/eval/file.h"

/*
Run a file `f`. After it is done, go into a REPL.
*/
noreturn void eval_file(FILE *f) {
	context_t *ctx = make_context();

	while (true) {
		if (f != stdin && getc(f) == EOF) {
			fclose(f);
			f = stdin;
		}
		else {
			fseek(f, -1, SEEK_CUR);
		}
		repl_loop(f, ctx);
	}
}
