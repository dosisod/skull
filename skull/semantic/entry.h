#pragma once

#include "skull/parse/ast_node.h"
#include "skull/semantic/shared.h"

_Bool validate_ast_tree(SemanticState *, AstNode *);
_Bool setup_and_validate_ast_sub_tree(SemanticState *, AstNode *);
