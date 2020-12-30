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
#include "skull/llvm/oper.h"
#include "skull/llvm/scope.h"
#include "skull/llvm/var.h"

#include "skull/llvm/assign.h"

extern LLVMModuleRef MODULE;
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern LLVMBuilderRef BUILDER;
extern Scope *SCOPE;

void llvm_assign_value_to_var(Variable *const, LLVMValueRef);

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	node_make_var(*node);

	Variable *var = SCOPE->vars[SCOPE->vars_used - 1];

	llvm_assign_value_to_var(
		var,
		llvm_node_to_value(var, (*node)->next)
	);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_var_assign(AstNode **node) {
	Variable *found_var = scope_find_var((*node)->token);

	if (found_var->is_const) {
		PANIC(ERR_REASSIGN_CONST, {
			.tok = (*node)->token
		});
	}

	llvm_assign_value_to_var(
		found_var,
		llvm_node_to_value(found_var, (*node)->next)
	);

	*node = (*node)->next;
}

/*
Based on `var` and `node`, try to make an LLVM value that is assignable to `var.
*/
LLVMValueRef llvm_node_to_value(const Variable *const var, const AstNode *const node) {
	if (!node) {
		PANIC(ERR_MISSING_ASSIGNMENT, { .var = var });
	}

	LLVMValueRef value = NULL;

	if (node->node_type == AST_NODE_IDENTIFIER) {
		value = llvm_assign_identifier(var, node);
	}
	else if (node->node_type == AST_NODE_ADD) {
		value = llvm_make_oper(var->type, node, &llvm_make_add);
	}
	else if (node->node_type == AST_NODE_SUB) {
		value = llvm_make_oper(var->type, node, &llvm_make_sub);
	}
	else if (node->node_type == AST_NODE_MULT) {
		value = llvm_make_oper(var->type, node, &llvm_make_mult);
	}
	else if (node->node_type == AST_NODE_DIV) {
		value = llvm_make_oper(var->type, node, &llvm_make_div);
	}
	else if (node->node_type == AST_NODE_FUNCTION) {
		Expr expr = llvm_make_function_call(node);

		if (expr.type != var->type) {
			PANIC(ERR_TYPE_MISMATCH, {
				.tok = node->token,
				.type = var->type
			});
		}

		value = expr.llvm_value;
	}
	else {
		value = token_to_simple_expr_typed(var->type, node->token).llvm_value;
	}

	if (!value) {
		PANIC(ERR_UNASSIGNABLE, {
			.tok = node->token,
			.real = var->name
		});
	}

	return value;
}

void llvm_assign_value_to_var(Variable *const var, LLVMValueRef value) {
	const bool is_first_assign = !var->llvm_value;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		(CURRENT_FUNC == MAIN_FUNC) :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->llvm_value = LLVMAddGlobal(
				MODULE,
				var->type->llvm_type(),
				var->name
			);

			LLVMSetLinkage(var->llvm_value, LLVMPrivateLinkage);

			LLVMSetInitializer(
				var->llvm_value,
				LLVMConstNull(var->type->llvm_type())
			);
		}
		else if (!is_global && !var->is_const) {
			var->llvm_value = LLVMBuildAlloca(
				BUILDER,
				var->type->llvm_type(),
				var->name
			);
		}

		var->is_global = is_global;
		var->is_const_lit = is_const_literal;
	}

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->llvm_value = value;
	}
	else {
		LLVMBuildStore(
			BUILDER,
			value,
			var->llvm_value
		);
	}
}

/*
Return LLVM for to load an existing identifier `node` to `var`.
*/
LLVMValueRef llvm_assign_identifier(const Variable *const var, const AstNode *const node) {
	Variable *var_found = NULL;
	Expr expr = token_to_expr(node->token, &var_found);

	if (var_found->type != var->type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = node->token,
			.type = var->type
		});
	}
	if (var == var_found) {
		PANIC(ERR_REDUNDANT_REASSIGN, {
			.tok = node->token,
			.var = var
		});
	}

	return expr.llvm_value;
}
