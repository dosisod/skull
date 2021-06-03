#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/func.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/compiler/scope.h"

#include "skull/codegen/assign.h"

static void assign_value_to_var(LLVMValueRef, Variable *const);
bool assert_sane_child(AstNode *);
static Type var_def_node_to_type(const AstNode *, bool *);
static Variable *node_to_var(const AstNode *const, bool *);

/*
Builds a variable definition from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_def(AstNode *node) {
	bool err = false;
	Variable *var = node_to_var(node, &err);
	if (err) return true;

	LLVMValueRef value = node_to_expr(
		var->type,
		node->var_def->expr_node,
		&err
	).value;

	if (err) {
		variable_no_warnings(var);
		return true;
	}

	assign_value_to_var(value, var);

	return false;
}

/*
Assign a to a variable from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_assign(AstNode *node) {
	Variable *var = scope_find_var(node->token);
	if (!var) return true;

	if (var->is_const) {
		FMT_ERROR(ERR_REASSIGN_CONST, {
			.tok = node->token
		});

		return true;
	}
	var->was_reassigned = true;

	bool err = false;

	LLVMValueRef value = node_to_expr(
		var->type,
		node->expr_node,
		&err
	).value;

	if (err) return true;

	assign_value_to_var(value, var);

	return false;
}

/*
Assign `value` to `var`.
*/
static void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->ref;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		SKULL_STATE.current_func == SKULL_STATE.main_func :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->ref = LLVMAddGlobal(
				SKULL_STATE.module,
				gen_llvm_type(var->type),
				var->name
			);

			LLVMSetLinkage(var->ref, LLVMPrivateLinkage);

			LLVMSetInitializer(
				var->ref,
				LLVMConstNull(gen_llvm_type(var->type))
			);
		}
		else if (!is_global && !var->is_const) {
			var->ref = LLVMBuildAlloca(
				SKULL_STATE.builder,
				gen_llvm_type(var->type),
				var->name
			);
		}
		else if (is_const_literal && !var->implicitly_typed) {
			FMT_WARN(WARN_TRIVIAL_TYPE, { .type = var->type });
		}

		var->is_global = is_global;
		var->is_const_lit = is_const_literal;
	}

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->ref = value;
	}
	else {
		LLVMBuildStore(
			SKULL_STATE.builder,
			value,
			var->ref
		);
	}
}

/*
Make and add a variable from `node` to Skull state.
*/
static Variable *node_to_var(const AstNode *const node, bool *err) {
	const Token *token = node->var_def->name_tok;
	Type type = NULL;

	if (node->var_def->is_implicit) {
		type = var_def_node_to_type(node, err);
		if (*err) return NULL;
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		type = find_type(type_name);
		free(type_name);

		if (!type) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = token->next });

			*err = true;
			return NULL;
		}
	}

	char32_t *const name = token_str(token);

	Variable *var = make_variable(
		type,
		name,
		node->var_def->is_const
	);
	free(name);
	var->implicitly_typed = node->var_def->is_implicit;

	if (find_func_by_name(var->name)) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC_AS_VAR, {
			.loc = &token->location,
			.var = var
		});

		variable_no_warnings(var);
		free_variable(var);

		*err = true;
		return NULL;
	}

	if (scope_add_var(&SKULL_STATE.scope, var)) {
		var->location = token->location;
		return var;
	}
	variable_no_warnings(var);
	free_variable(var);

	FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .tok = token });

	*err = true;
	return NULL;
}

/*
Returns the left-most expr that is either a constant, variable, or function.
*/
static __attribute__((pure)) const AstNodeExpr *leftmost_expr(
	const AstNodeExpr *expr
) {
	while (expr->oper != EXPR_CONST &&
		expr->oper != EXPR_IDENTIFIER &&
		expr->oper != EXPR_FUNC
	) {
		if (expr->oper == EXPR_UNARY_NEG) {
			expr = expr->rhs.expr;
		}
		else {
			expr = expr->lhs.expr;
		}
	}

	return expr;
}

/*
Return a variable type based on `node`.
*/
static Type var_def_node_to_type(const AstNode *node, bool *err) {
	AstNode *expr_node = node->var_def->expr_node;
	TokenType token_type = expr_node->token->type;

	if (expr_node->type == AST_NODE_EXPR) {
		const AstNodeExpr *expr = expr_node->expr;

		switch (expr->oper) {
			case EXPR_NOT:
			case EXPR_IS:
			case EXPR_ISNT:
			case EXPR_LESS_THAN:
			case EXPR_GTR_THAN:
			case EXPR_LESS_THAN_EQ:
			case EXPR_GTR_THAN_EQ:
			case EXPR_AND:
			case EXPR_OR:
			case EXPR_XOR:
				return TYPE_BOOL;
			default: break;
		}

		expr = leftmost_expr(expr);

		if (expr->oper == EXPR_CONST) {
			token_type = expr->lhs.tok->type;
		}
		else if (expr->oper == EXPR_IDENTIFIER) {
			const Variable *var = scope_find_var(expr->lhs.tok);
			if (!var) {
				*err = true;
				return NULL;
			}

			return var->type;
		}
		else if (expr->oper == EXPR_FUNC) {
			const Token *func_name_token = expr->func_call->func_name_tok;

			char *const func_name = token_mbs_str(func_name_token);

			FunctionDeclaration *const function = \
				find_func_by_name(func_name);

			free(func_name);

			if (!function) {
				FMT_ERROR(ERR_MISSING_DECLARATION, { .tok = func_name_token });

				*err = true;
				return NULL;
			}

			Type type = function->return_type;

			if (type == TYPE_VOID) {
				FMT_ERROR(ERR_NO_VOID_ASSIGN, {
					.loc = &func_name_token->location,
					.real = token_mbs_str(node->token)
				});

				// suppress errors
				function->was_called = true;

				*err = true;
			}

			return type;
		}
	}

	switch (token_type) {
		case TOKEN_BOOL_CONST: return TYPE_BOOL;
		case TOKEN_INT_CONST: return TYPE_INT;
		case TOKEN_FLOAT_CONST: return TYPE_FLOAT;
		case TOKEN_RUNE_CONST: return TYPE_RUNE;
		case TOKEN_STR_CONST: return TYPE_STR;
		default: break;
	}

	FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token });

	return NULL;
}
