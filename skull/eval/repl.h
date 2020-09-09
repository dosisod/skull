#pragma once

#include <stdio.h>

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/tokenize.h"

char *repl_read(FILE *);

const char32_t *repl_make_var(const AstNode *, Context *, _Bool);
const char32_t *repl_eval(const char *, Context *);

void repl_loop(FILE *, Context *);