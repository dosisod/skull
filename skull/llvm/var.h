#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

#define SCOPE_FIND_VAR(var, token, var_name) \
	char32_t *const (var_name)= token_str((token)); \
	Variable *const (var) = scope_find_name(scope, (var_name)); \
	if (!(var)) { \
		PANIC(ERR_VAR_NOT_FOUND, { .str = (var_name) }); \
	}

void node_make_var(const AstNode *const, Scope *const);

const Type *token_type_to_type(const Token *);

LLVMValueRef llvm_parse_var(const Variable *const, const Token *const);
LLVMValueRef llvm_parse_token(const Token *const);
