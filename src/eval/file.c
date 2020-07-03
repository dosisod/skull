#include <stdlib.h>

#include "../errors.h"
#include "context.h"
#include "repl.h"

#include "file.h"

/*
Run a file `f`. After it is done, go into a REPL.
*/
noreturn void eval_file(FILE *f) {
	context_t *ctx=make_context();

	while (true) {
		if (f!=stdin && getc(f)==EOF) {
			fclose(f);
			f=stdin;
		}
		else {
			fseek(f, -1, SEEK_CUR);
		}
		repl_loop(f, ctx);
	}
}
