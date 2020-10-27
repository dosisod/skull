#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
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

#include "skull/llvm/assign.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

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
		PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
	}

	if (found_var->is_const) {
		PANIC(FMT_ERROR(U"cannot reassign const variable \"%\"", { .real = var_name }));
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
		PANIC(FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .var = var }));
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
		llvm_make_math_oper(var, node, &llvm_make_add, U"cannot add \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_SUB_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_sub, U"cannot subtract \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_MULT_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_mult, U"cannot multiply \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_DIV_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_div, U"cannot divide \"%\" and \"%\"");
		free(var_name);
		return;
	}

	char32_t *err = NULL;

	if (var->type == &TYPE_INT && node->node_type == AST_NODE_INT_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_INT(eval_integer(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_FLOAT && node->node_type == AST_NODE_FLOAT_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_FLOAT(eval_float(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_BOOL && node->node_type == AST_NODE_BOOL_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_BOOL(eval_bool(node->token)),
			var->alloca
		);
	}
	else if (var->type == &TYPE_RUNE && node->node_type == AST_NODE_RUNE_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_RUNE(eval_rune(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_STR && node->node_type == AST_NODE_STR_CONST) {
		SkullStr str = eval_str(node->token, &err);

		char *const mbs = c32stombs(str);
		const unsigned len = (unsigned)strlen(mbs);

		LLVMBuildStore(
			builder,
			LLVMConstString(mbs, len, false),
			LLVMBuildAlloca(
				builder,
				LLVMArrayType(
					LLVMInt8Type(),
					len + 1
				),
				""
			)
		);
		free(mbs);

		LLVMTypeRef str_ptr = LLVMPointerType(LLVMInt8Type(), 0);

		LLVMBuildStore(
			builder,
			LLVMBuildBitCast(
				builder,
				var->alloca,
				str_ptr,
				""
			),
			var->alloca
		);
	}
	else {
		PANIC(FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type }));
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
		PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup }));
	}
	free(lookup);
	if (var_found->type != var->type) {
		PANIC(FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type }));
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
