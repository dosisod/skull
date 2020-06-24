#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include "context.h"
#include "repl.h"

noreturn void eval_file(FILE *f) {
	context_t *ctx=make_context();
	bool done=false;

	while (true) {
		if (!done && getc(f)==-1) {
			fclose(f);
			f=stdin;
			done=true;
		}
		else {
			fseek(f, -1, SEEK_CUR);
		}

		char32_t *line=repl_read(f);

		const char32_t *tmp=repl_eval(line, ctx);
		char *output=c32stombs(tmp);
		if (output!=NULL) {
			printf("%s\n", output);
		}

		free(output);
		free(line);
	}
}
