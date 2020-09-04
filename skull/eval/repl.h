#pragma once

#include <stdio.h>

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/tokenize.h"

char32_t *repl_read(FILE *);

const char32_t *repl_make_var(const AstNode *, Context *, bool);
const char32_t *repl_eval(const char32_t *, Context *);

void repl_loop(FILE *, Context *);