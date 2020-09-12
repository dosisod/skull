#pragma once

#include <uchar.h>

#include "skull/eval/scope.h"
#include "skull/parse/ast/node.h"

_Bool is_func_name(const AstNode *);
_Bool is_func_name_str(const char32_t *);

const char32_t *func_clear(AstNode *);
const char32_t *func_print(AstNode *, Scope *);