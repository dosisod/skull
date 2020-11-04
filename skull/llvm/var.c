#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"

#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"

#include "skull/llvm/aliases.h"
#include "skull/parse/classify.h"

#include "skull/llvm/var.h"

extern LLVMBuilderRef builder;

const Type *token_type_to_type(const Token *token) {
	if (token->token_type == TOKEN_INT_CONST) {
		return &TYPE_INT;
	}
	if (token->token_type == TOKEN_FLOAT_CONST) {
		return &TYPE_FLOAT;
	}
	if (token->token_type == TOKEN_RUNE_CONST) {
		return &TYPE_RUNE;
	}
	if (token->token_type == TOKEN_BOOL_CONST) {
		return &TYPE_BOOL;
	}
	if (token->token_type == TOKEN_STR_CONST) {
		return &TYPE_STR;
	}
	return NULL;
}

/*
Make and add a variable from `node` to `scope`.
*/
void node_make_var(const AstNode *const node, Scope *const scope) {
	const Token *token = node->token;

	const bool is_const = ATTR(AstNodeVarDef, node, is_const);
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		PANIC(ERR_MISSING_ASSIGNMENT, { .tok = token });
	}

	char32_t *const name = token_str(token);
	Variable *var = NULL;

	if (ATTR(AstNodeVarDef, node, is_implicit)) {
		const Type *type = token_type_to_type(node->next->token);

		if (type) {
			// fallthrough
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER) {
			SCOPE_FIND_VAR(new_var, node->next->token, lookup);

			free(lookup);
			type = new_var->type;
		}
		else {
			free(name);
			PANIC("invalid input: \"%s\"\n", { .tok = node->next->token });
		}
		var = make_variable(type, name, is_const);
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		var = make_variable(find_type(type_name), name, is_const);
		free(type_name);
	}

	if (scope_add_var(scope, var)) {
		free(name);
		return;
	}
	free_variable(var);

	PANIC("variable \"%s\" already defined\n", { .str = name });
}

/*
Make an `LLVMValueRef` for a given `var` from `token`.
*/
LLVMValueRef llvm_parse_var(const Variable *const var, const Token *const token) {
	if (var->type == token_type_to_type(token)) {
		return llvm_parse_token(token);
	}

	PANIC(ERR_TYPE_MISMATCH, { .type = var->type });
}

/*
Make an `LLVMValueRef` from `token`.
*/
LLVMValueRef llvm_parse_token(const Token *const token) {
	if (token->token_type == TOKEN_INT_CONST) {
		return LLVM_INT(eval_integer(token));
	}
	if (token->token_type == TOKEN_FLOAT_CONST) {
		return LLVM_FLOAT(eval_float(token));
	}
	if (token->token_type == TOKEN_BOOL_CONST) {
		return LLVM_BOOL(eval_bool(token));
	}
	if (token->token_type == TOKEN_RUNE_CONST) {
		return LLVM_RUNE(eval_rune(token));
	}
	if (token->token_type == TOKEN_STR_CONST) {
		SkullStr str = eval_str(token);
		char *const mbs = c32stombs(str);

		LLVMValueRef ret = LLVMBuildBitCast(
			builder,
			LLVMBuildGlobalString(builder, mbs, ""),
			LLVMPointerType(LLVMInt8Type(), 0),
			""
		);

		free(mbs);
		free(str);

		return ret;
	}

	return NULL;
}
