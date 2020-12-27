#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast_node.h"

const Type *token_type_to_type(const Token *const);

LLVMValueRef llvm_token_get_value(const Token *const, Variable **);

void node_make_var(const AstNode *const);

LLVMValueRef llvm_parse_token_typed(const Type *const, const Token *const);
LLVMValueRef llvm_parse_token(const Token *const);
