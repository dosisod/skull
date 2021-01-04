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

#include "skull/llvm/assign.h"

extern LLVMModuleRef MODULE;
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern LLVMBuilderRef BUILDER;
extern Scope *SCOPE;

void llvm_assign_value_to_var(Variable *const, LLVMValueRef);

void llvm_make_type_alias(AstNode **node) {
	char *type_name = token_mbs_str((*node)->token->next->next);

	const bool added = add_alias(
		(Type *)find_type(type_name),
		token_mbs_str((*node)->token)
	);

	free(type_name);

	if (!added) {
		PANIC(ERR_ALIAS_ALREADY_DEFINED, {
			.tok = (*node)->token
		});
	}
}

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	node_make_var(*node);

	Variable *var = SCOPE->vars[SCOPE->vars_used - 1];

	llvm_assign_value_to_var(
		var,
		node_to_expr(var->type, (*node)->next, var).llvm_value
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
		node_to_expr(found_var->type, (*node)->next, found_var).llvm_value
	);

	*node = (*node)->next;
}

/*
Create an expression from `node` with type `type`.

Optionally pass `var` if expression is going to be assigned to a variable.
*/
Expr node_to_expr(const Type *const type, const AstNode *const node, const Variable *const var) {
	if (!node) {
		PANIC(ERR_MISSING_ASSIGNMENT, { .var = var });
	}

	Expr expr;

	if (node->node_type == AST_NODE_IDENTIFIER) {
		expr = (Expr){
			.llvm_value = llvm_assign_identifier(type, node, var),
			.type = type
		};
	}
	else if (node->node_type == AST_NODE_ADD) {
		expr = (Expr){
			.llvm_value = llvm_make_oper(type, node, &llvm_make_add),
			.type = type
		};
	}
	else if (node->node_type == AST_NODE_SUB) {
		expr = (Expr){
			.llvm_value = llvm_make_oper(type, node, &llvm_make_sub),
			.type = type
		};
	}
	else if (node->node_type == AST_NODE_MULT) {
		expr = (Expr){
			.llvm_value = llvm_make_oper(type, node, &llvm_make_mult),
			.type = type
		};
	}
	else if (node->node_type == AST_NODE_DIV) {
		expr = (Expr){
			.llvm_value = llvm_make_oper(type, node, &llvm_make_div),
			.type = type
		};
	}
	else if (node->node_type == AST_NODE_FUNCTION) {
		expr = llvm_make_function_call(node);

		if (expr.type != type) {
			PANIC(ERR_TYPE_MISMATCH, {
				.tok = node->token,
				.type = type
			});
		}
	}
	else {
		expr = token_to_simple_expr_typed(type, node->token);
	}

	if (!expr.llvm_value) {
		PANIC(ERR_INVALID_EXPR, { .tok = node->token });
	}

	return expr;
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
Return LLVM for to load an existing identifier `node` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.
*/
LLVMValueRef llvm_assign_identifier(const Type *const type, const AstNode *const node, const Variable *const var) {
	Variable *var_found = NULL;
	Expr expr = token_to_expr(node->token, &var_found);

	if (var_found->type != type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = node->token,
			.type = type
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
