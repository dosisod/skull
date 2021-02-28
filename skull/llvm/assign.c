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

Expr gen_expr_bool_expr(const AstNode *const);

Expr gen_expr_identifier(
	const Type *const,
	const AstNode *const,
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

	if (node->type == AST_NODE_IDENTIFIER) {
		expr = gen_expr_identifier(type, node, var);
	}
	else if (node->type == AST_NODE_OPER_EXPR) {
		const TokenType token_type = ATTR(AstNodeOper, node, oper);

		if (token_type == TOKEN_OPER_PLUS) {
			expr = gen_expr_oper(type, node, &gen_expr_add);
		}
		else if (token_type == TOKEN_OPER_MINUS) {
			expr = gen_expr_oper(type, node, &gen_expr_sub);
		}
		else if (token_type == TOKEN_OPER_MULT) {
			expr = gen_expr_oper(type, node, &gen_expr_mult);
		}
		else if (token_type == TOKEN_OPER_DIV) {
			expr = gen_expr_oper(type, node, &gen_expr_div);
		}
		else if (token_type == TOKEN_OPER_MOD) {
			expr = gen_expr_oper(type, node, &gen_expr_mod);
		}
		else if (token_type == TOKEN_OPER_LSHIFT) {
			expr = gen_expr_oper(type, node, &gen_expr_lshift);
		}
		else if (token_type == TOKEN_OPER_POW) {
			expr = gen_expr_oper(type, node, &gen_expr_pow);
		}
		else if (token_type == TOKEN_OPER_RSHIFT) {
			expr = gen_expr_oper(type, node, &gen_expr_rshift);
		}
	}
	else if (node->type == AST_NODE_BOOL_EXPR) {
		expr = gen_expr_bool_expr(node);
	}
	else if (node->type == AST_NODE_FUNCTION) {
		expr = gen_expr_function_call(node, type);
	}
	else if (node->type == AST_NODE_CONST) {
		expr = token_to_simple_expr_typed(type, node->token);
	}

	if (!expr.llvm_value) {
		PANIC(ERR_INVALID_EXPR, { .tok = node->token });
	}

	return expr;
}

/*
Build LLVM code to handle boolean expressions from `node`.
*/
Expr gen_expr_bool_expr(const AstNode *const node) {
	const Token *const lhs = ATTR(AstNodeOper, node, lhs);
	const TokenType oper = ATTR(AstNodeOper, node, oper);
	const Token *const rhs = ATTR(AstNodeOper, node, rhs);

	if (oper == TOKEN_OPER_NOT) {
		const Expr rhs_expr = token_to_expr(rhs, NULL);

		if (rhs_expr.type != &TYPE_BOOL) {
			PANIC(ERR_NON_BOOL_EXPR, { .tok = rhs });
		}

		return (Expr){
			.llvm_value = LLVMBuildNot(
				SKULL_STATE.builder,
				rhs_expr.llvm_value,
				""
			),
			.type = &TYPE_BOOL
		};
	}

	const Expr lhs_expr = token_to_expr(lhs, NULL);
	const Expr rhs_expr = token_to_expr(rhs, NULL);

	if (lhs_expr.type != rhs_expr.type) {
		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = rhs, .type = lhs_expr.type },
			{ .type = rhs_expr.type }
		);
	}

	Operation *func = NULL;

	if (oper == TOKEN_OPER_IS) {
		func = gen_expr_is;
	}
	else if (oper == TOKEN_OPER_ISNT) {
		func = gen_expr_is_not;
	}
	else if (oper == TOKEN_OPER_LESS_THAN) {
		func = gen_expr_less_than;
	}
	else if (oper == TOKEN_OPER_GTR_THAN) {
		func = gen_expr_gtr_than;
	}
	else if (oper == TOKEN_OPER_LESS_THAN_EQ) {
		func = gen_expr_less_than_eq;
	}
	else if (oper == TOKEN_OPER_GTR_THAN_EQ) {
		func = gen_expr_gtr_than_eq;
	}
	else {
		if (lhs_expr.type != &TYPE_BOOL) {
			PANIC(ERR_BOOL_ONLY, { .tok = lhs });
		}

		if (oper == TOKEN_OPER_AND) {
			func = gen_expr_and;
		}
		else if (oper == TOKEN_OPER_OR) {
			func = gen_expr_or;
		}
		else if (oper == TOKEN_OPER_XOR) {
			func = gen_expr_xor;
		}
	}

	LLVMValueRef value = NULL;

	if (func) {
		value = func(
			lhs_expr.type,
			lhs_expr.llvm_value,
			rhs_expr.llvm_value
		).llvm_value;
	}

	if (!value) {
		PANIC(ERR_NOT_COMPARIBLE, { .tok = lhs });
	}

	return (Expr){
		.llvm_value = value,
		.type = &TYPE_BOOL
	};
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
Return expression for identifier `node` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.

If `type` is not set, the expression type will not be checked.
*/
Expr gen_expr_identifier(
	const Type *const type,
	const AstNode *const node,
	const Variable *const var
) {
	Variable *var_found = NULL;
	const Expr expr = token_to_expr(node->token, &var_found);

	if (type && var_found->type != type) {
		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = node->token, .type = type },
			{ .type = var_found->type }
		);
	}
	if (var == var_found) {
		PANIC(ERR_REDUNDANT_REASSIGN, {
			.tok = node->token,
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
