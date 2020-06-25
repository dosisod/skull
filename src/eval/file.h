#include <stdio.h>
#include <stdnoreturn.h>

/*
Run a file. After it is done, go into a REPL.
*/
noreturn void eval_file(FILE *f);
