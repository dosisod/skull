#pragma once

#include <stdbool.h>
#include <uchar.h>

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"

bool is_func_name(const ast_node_t *);
bool is_func_name_str(const char32_t *);

const char32_t *func_clear(ast_node_t *);
const char32_t *func_print(ast_node_t *, context_t *);