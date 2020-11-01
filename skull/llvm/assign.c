#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/math.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/assign.h"

Scope *scope;
LLVMBuilderRef builder;

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	node_make_var(*node, scope);

	llvm_make_assign_(scope->vars[scope->vars_used - 1], (*node)->next);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_assign(AstNode **node) {
	char32_t *const var_name = token_str((*node)->token);
	Variable *const found_var = scope_find_name(scope, var_name);

	if (!found_var) {
		PANIC(ERR_VAR_NOT_FOUND, { .str = var_name });
	}

	if (found_var->is_const) {
		PANIC("cannot reassign const variable \"%s\"\n", { .str = var_name });
	}
	free(var_name);

	llvm_make_assign_(found_var, (*node)->next);

	*node = (*node)->next;
}

/*
Internal function to build LLVM assignment from `node` to `var.
*/
void llvm_make_assign_(Variable *const var, const AstNode *const node) {
	if (!node) {
		PANIC(ERR_MISSING_ASSIGNMENT, { .var = var });
	}

	if (node->node_type == AST_NODE_IDENTIFIER) {
		llvm_assign_identifier(var, node);
		return;
	}

	char *const var_name = c32stombs(var->name);

	if (!var->alloca) {
		var->alloca = LLVMBuildAlloca(
			builder,
			var->type->llvm_type(),
			var_name
		);
	}

	if (node->node_type == AST_NODE_ADD_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_add, "cannot add \"%s\" and \"%s\"\n");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_SUB_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_sub, "cannot subtract \"%s\" and \"%s\"\n");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_MULT_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_mult, "cannot multiply \"%s\" and \"%s\"\n");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_DIV_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_div, "cannot divide \"%s\" and \"%s\"\n");
		free(var_name);
		return;
	}

	if (var->type == &TYPE_STR && node->token->token_type == TOKEN_STR_CONST) {
		SkullStr str = eval_str(node->token);

		char *const mbs = c32stombs(str);
		const unsigned len = (unsigned)strlen(mbs);

		LLVMValueRef str_arr = LLVMBuildAlloca(
			builder,
			LLVMArrayType(
				LLVMInt8Type(),
				len + 1
			),
			""
		);

		LLVMBuildStore(
			builder,
			LLVMConstString(mbs, len, false),
			str_arr
		);
		free(mbs);

		LLVMBuildStore(
			builder,
			LLVMBuildBitCast(
				builder,
				str_arr,
				LLVMPointerType(LLVMInt8Type(), 0),
				""
			),
			var->alloca
		);
	}
	else {
		LLVMBuildStore(
			builder,
			llvm_parse_var(var, node->token),
			var->alloca
		);
	}

	free(var_name);
}

/*
Build LLVM to assign an existing identifier `node` to `var`.
*/
void llvm_assign_identifier(Variable *const var, const AstNode *const node) {
	char32_t *const lookup = token_str(node->token);
	const Variable *const var_found = scope_find_name(scope, lookup);

	if (!var_found) {
		PANIC(ERR_VAR_NOT_FOUND, { .str = lookup });
	}
	free(lookup);
	if (var_found->type != var->type) {
		PANIC(ERR_TYPE_MISMATCH, { .type = var->type });
	}

	if (!var->alloca) {
		var->alloca = LLVMBuildAlloca(
			builder,
			var->type->llvm_type(),
			c32stombs(var->name)
		);
	}

	LLVMValueRef load = LLVMBuildLoad2(
		builder,
		var->type->llvm_type(),
		var_found->alloca,
		""
	);

	LLVMBuildStore(
		builder,
		load,
		var->alloca
	);
}
