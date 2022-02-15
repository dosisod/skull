#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/mangle.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/func.h"


static bool is_void_func_assign(const AstNode *);
static Symbol *node_to_var(const AstNode *const);
static bool is_expr_compatible_with_var(const AstNodeExpr *, const Variable *);
static bool is_redundant_reassign(const AstNodeVarAssign *);


bool validate_stmt_var_def(const AstNode *node) {
	char *name = token_to_mbs_str(node->token_end->next);
	const Type *type = find_type(name);
	free(name);

	if (type) return validate_stmt_type_alias(node);

	AstNodeExpr *expr = node->var_def->expr;
	if (!validate_expr(expr)) return false;

	Symbol *symbol = node_to_var(node);
	if (!symbol || !symbol->var) return false;

	Variable *var = symbol->var;

	var->is_defined = true;
	var->expr = expr;

	node->var_def->symbol = symbol;

	if (!is_expr_compatible_with_var(expr, var)) return false;

	if (expr->oper == EXPR_CONST && !var->implicitly_typed) {
		bool err = false;
		FMT_WARN(err, WARN_TRIVIAL_TYPE, { .type = var->type });
		if (err) return false;
	}

	return true;
}

bool validate_stmt_var_assign(const AstNode *node) {
	Symbol *symbol = scope_find_var(node->token);
	if (!symbol || !symbol->var) return false;

	node->var_assign->symbol = symbol;

	if (symbol->var->is_const) {
		FMT_ERROR(ERR_REASSIGN_CONST, {
			.tok = node->token
		});

		return false;
	}

	symbol->var->was_reassigned = true;

	if (!validate_expr(node->var_assign->expr)) return false;

	return (
		is_expr_compatible_with_var(node->var_assign->expr, symbol->var) &&
		!is_redundant_reassign(node->var_assign)
	);
}

/*
Make and add a symbol (as a variable) from `node` to the Skull state.

Return `NULL` if an error occurred.
*/
static Symbol *node_to_var(const AstNode *const node) {
	const Token *token = node->var_def->name_tok;
	const Type *type = node->var_def->expr->type;

	if (is_void_func_assign(node)) return NULL;

	if (!node->var_def->is_implicit) {
		type = token_to_type(token->next);

		if (!type) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = token->next });

			return NULL;
		}
	}

	char32_t *const name = token_to_string(token);

	Variable *var = make_variable(
		type,
		name,
		node->var_def->is_const
	);
	free(name);
	var->implicitly_typed = node->var_def->is_implicit;
	var->is_exported = node->var_def->is_exported;
	var->location = token->location;

	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);

	var->linkage_name = var->is_exported ? mangle_name(var->name) : var->name;

	*symbol = (Symbol){
		.name = var->name,
		.expr_type = var->type,
		.location = &var->location,
		.type = SYMBOL_VAR,
		.var = var
	};

	if (scope_add_symbol(symbol)) {
		return symbol;
	}

	var->name = NULL;
	variable_disable_warnings(var);
	free_variable(var);
	free(symbol);

	return NULL;
}

static bool is_void_func_assign(const AstNode *node) {
	const AstNodeExpr *expr = node->var_def->expr;

	if (expr->oper == EXPR_FUNC && expr->type == &TYPE_VOID) {
		FMT_ERROR(ERR_NO_VOID_ASSIGN, {
			.loc = &expr->lhs.func_call->func_name_tok->location,
			.real = token_to_mbs_str(node->token)
		});

		// suppress errors
		expr->lhs.func_call->symbol->func->was_called = true;

		return true;
	}

	return false;
}

static bool is_expr_compatible_with_var(
	const AstNodeExpr *expr,
	const Variable *var
) {
	if (expr->type != var->type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{
				.loc = find_expr_node_location(expr),
				.type = var->type
			},
			{ .type = expr->type }
		);
		return false;
	}

	return true;
}

static bool is_redundant_reassign(const AstNodeVarAssign *var_assign) {
	const AstNodeExpr *expr = var_assign->expr;

	if (expr->oper == EXPR_IDENTIFIER && expr->symbol == var_assign->symbol) {
		FMT_ERROR(ERR_REDUNDANT_REASSIGN, { .tok = expr->lhs.tok });

		return true;
	}

	return false;
}
