#pragma once

#include <stdio.h>

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/tokenize.h"

char32_t *repl_read(FILE *);

const char32_t *repl_make_var(const ast_node_t *, context_t *, bool);
const char32_t *repl_eval(const char32_t *, context_t *);

void repl_loop(FILE *, context_t *);