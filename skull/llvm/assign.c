#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/eval/types/defs.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/func.h"
#include "skull/llvm/math.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/assign.h"

extern Scope *SCOPE;
extern LLVMBuilderRef BUILDER;

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	node_make_var(*node);

	llvm_make_assign_(SCOPE->vars[SCOPE->vars_used - 1], (*node)->next);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_assign(AstNode **node) {
	SCOPE_FIND_VAR(found_var, (*node)->token, var_name);

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

	char *const var_name = c32stombs(var->name);

	if (!var->alloca && !var->is_const) {
		var->alloca = LLVMBuildAlloca(
			BUILDER,
			var->type->llvm_type(),
			var_name
		);
	}

	LLVMValueRef value = NULL;

	if (node->node_type == AST_NODE_IDENTIFIER) {
		value = llvm_assign_identifier(var, node);
	}
	else if (node->node_type == AST_NODE_ADD) {
		value = llvm_make_math_oper(var->type, node, &llvm_make_add);
	}
	else if (node->node_type == AST_NODE_SUB) {
		value = llvm_make_math_oper(var->type, node, &llvm_make_sub);
	}
	else if (node->node_type == AST_NODE_MULT) {
		value = llvm_make_math_oper(var->type, node, &llvm_make_mult);
	}
	else if (node->node_type == AST_NODE_DIV) {
		value = llvm_make_math_oper(var->type, node, &llvm_make_div);
	}
	else if (node->node_type == AST_NODE_FUNCTION) {
		value = llvm_make_function_call(node);
	}
	else {
		value = llvm_parse_var(var->type, node->token);
	}

	if (!value) {
		PANIC("unable to assign value to variable \"%s\"\n", { .real = var_name });
	}

	free(var_name);

	if (var->is_const) {
		var->alloca = value;
	}
	else {
		LLVMBuildStore(
			BUILDER,
			value,
			var->alloca
		);
	}
}

/*
Return LLVM for to load an existing identifier `node` to `var`.
*/
LLVMValueRef llvm_assign_identifier(Variable *const var, const AstNode *const node) {
	SCOPE_FIND_VAR(var_found, node->token, lookup);
	free(lookup);

	if (var_found->type != var->type) {
		PANIC(ERR_TYPE_MISMATCH, { .type = var->type });
	}
	if (var == var_found) {
		PANIC("redundant assignment of variable \"%s\" to itself\n", { .var = var });
	}

	return llvm_var_get_value(var_found);
}
