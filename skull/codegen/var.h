#pragma once

#include <stdbool.h>

#include "skull/codegen/llvm_fwd_decl.h"
#include "skull/compiler/variable.h"
#include "skull/parse/ast_node.h"

Variable *node_to_var(const AstNode *const, bool *);
