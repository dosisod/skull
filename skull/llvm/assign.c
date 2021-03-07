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
#include "skull/llvm/shared.h"

#include "skull/llvm/assign.h"

void assign_value_to_var(LLVMValueRef, Variable *const);

/*
Builds a variable from `node`.
*/
void gen_stmt_var_def(AstNode **node) {
	Variable *var = node_to_var(*node);

	assign_value_to_var(
		node_to_expr(var->type, (*node)->next, var).llvm_value,
		var
	);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void gen_stmt_var_assign(AstNode **node) {
	Variable *found_var = scope_find_var((*node)->token);

	if (found_var->is_const) {
		PANIC(ERR_REASSIGN_CONST, {
			.tok = (*node)->token
		});
	}

	assign_value_to_var(
		node_to_expr(found_var->type, (*node)->next, found_var).llvm_value,
		found_var
	);

	*node = (*node)->next;
}

Expr gen_expr_identifier(
	const Type *const,
	const Token *const,
	const Variable *const
);

/*
Create an expression from `node` with type `type`.

Optionally pass `var` if expression is going to be assigned to a variable.
*/
Expr node_to_expr(
	const Type *const type,
	const AstNode *const node,
	const Variable *const var
) {
	Expr expr = {0};

	if (node->type == AST_NODE_EXPR) {
		const ExprType oper = ATTR(AstNodeExpr, node, oper);

		if (oper == EXPR_IDENTIFIER) {
			expr = gen_expr_identifier(type, node->token, var);
		}
		else if (oper == EXPR_NOT) {
			expr = gen_expr_not(type, ATTR(AstNodeExpr, node, rhs));
		}
		else if (oper == EXPR_CONST) {
			expr = token_to_simple_expr_typed(type, node->token);
		}
		else {
			Operation *func = NULL;

			switch (oper) {
				case EXPR_ADD: func = &gen_expr_add; break;
				case EXPR_SUB: func = &gen_expr_sub; break;
				case EXPR_MULT: func = &gen_expr_mult; break;
				case EXPR_DIV: func = &gen_expr_div; break;
				case EXPR_MOD: func = &gen_expr_mod; break;
				case EXPR_LSHIFT: func = &gen_expr_lshift; break;
				case EXPR_POW: func = &gen_expr_pow; break;
				case EXPR_RSHIFT: func = &gen_expr_rshift; break;
				case EXPR_IS: func = gen_expr_is; break;
				case EXPR_ISNT: func = gen_expr_is_not; break;
				case EXPR_LESS_THAN: func = gen_expr_less_than; break;
				case EXPR_GTR_THAN: func = gen_expr_gtr_than; break;
				case EXPR_LESS_THAN_EQ: func = gen_expr_less_than_eq; break;
				case EXPR_GTR_THAN_EQ: func = gen_expr_gtr_than_eq; break;
				case EXPR_AND: func = gen_expr_and; break;
				case EXPR_OR: func = gen_expr_or; break;
				case EXPR_XOR: func = gen_expr_xor; break;
				default: break;
			}

			if (func) {
				expr = gen_expr_oper(type, node, func);
			}
		}
	}
	else if (node->type == AST_NODE_FUNCTION) {
		expr = gen_expr_function_call(node, type);
	}

	if (!expr.llvm_value) {
		PANIC(ERR_INVALID_EXPR, { .tok = node->token });
	}

	return expr;
}

/*
Assign `value` to `var`.
*/
void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->llvm_value;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		(SKULL_STATE.current_func == SKULL_STATE.main_func) :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->llvm_value = LLVMAddGlobal(
				SKULL_STATE.module,
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
				SKULL_STATE.builder,
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
			SKULL_STATE.builder,
			value,
			var->llvm_value
		);
	}
}

/*
Return expression for identifier `token` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.

If `type` is not set, the expression type will not be checked.
*/
Expr gen_expr_identifier(
	const Type *const type,
	const Token *const token,
	const Variable *const var
) {
	Variable *var_found = NULL;
	const Expr expr = token_to_expr(token, &var_found);

	if (type && var_found->type != type) {
		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = token, .type = type },
			{ .type = var_found->type }
		);
	}
	if (var == var_found) {
		PANIC(ERR_REDUNDANT_REASSIGN, {
			.tok = token,
			.var = var
		});
	}

	return expr;
}

/*
Create a type alias from `node`.
*/
void create_type_alias(AstNode **node) {
	const Token *const token = (*node)->token;

	char *type_name = token_mbs_str(token->next->next);

	const bool added = add_alias(
		(Type *)find_type(type_name),
		token_mbs_str(token)
	);

	free(type_name);

	if (!added) {
		PANIC(ERR_ALIAS_ALREADY_DEFINED, { .tok = token });
	}
}
