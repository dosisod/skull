#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast_node.h"

/*
Find the variable `var` stored in `token`.

Variable name is stored in `var_name`, and must be freed.
*/
#define SCOPE_FIND_VAR(var, token, var_name) \
	char *const (var_name) = token_mbs_str((token)); \
	Variable *const (var) = scope_find_name(SCOPE, (var_name)); \
	if (!(var)) { \
		PANIC(ERR_VAR_NOT_FOUND, { .real = (var_name) }); \
	}

const Type *token_type_to_type(const Token *);

LLVMValueRef llvm_var_get_value(const Variable *const);

void node_make_var(const AstNode *const);

LLVMValueRef llvm_parse_token_typed(const Type *const, const Token *const);
LLVMValueRef llvm_parse_token(const Token *const);
