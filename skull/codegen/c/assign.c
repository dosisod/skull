#include <stddef.h>
#include <stdlib.h>

#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/assign.h"

static CStmt gen_stmt_var_assign_c_(
	const char *,
	const AstNodeExpr *,
	SkullStateC *
);
static CStmt gen_stmt_global_var(const AstNode *, CType type, SkullStateC *);
static CStmt gen_stmt_stack_var(const AstNode *, CType type, SkullStateC *);

CStmt gen_stmt_var_assign_c(const AstNode *node, SkullStateC *state) {
	return gen_stmt_var_assign_c_(
		node->var_assign->var->linkage_name,
		node->var_assign->expr,
		state
	);
}

CStmt gen_stmt_var_def_c(const AstNode *node, SkullStateC *state) {
	const Variable *var = node->var_def->var;

	if (!var) return NULL;

	CType type = skull_type_to_c_type(var->type);

	if (state->indent_lvl == 1) {
		return gen_stmt_global_var(node, type, state);
	}

	return gen_stmt_stack_var(node, type, state);
}

static CStmt gen_stmt_var_assign_c_(
	const char *name,
	const AstNodeExpr *expr,
	SkullStateC *state
) {
	CExpr expr_str = gen_expr_c(expr, state);
	CStmt stmt = uvsnprintf("%s = %s;", name, expr_str);

	free(expr_str);
	return stmt;
}

static CStmt gen_stmt_global_var(
	const AstNode *node,
	CType type,
	SkullStateC *state
) {
	const Variable *var = node->var_def->var;

	if (var->expr->is_const_expr) {
		CExpr value = gen_expr_c(var->expr, state);

		const char *fmt = NULL;

		if (var->is_exported) {
			if (var->is_const) fmt = "%s\nconst %s %s = %s;";
			else fmt = "%s\n%s %s = %s;";
		}
		else {
			if (var->is_const) fmt = "%s\nstatic const %s %s = %s;";
			else fmt = "%s\nstatic %s %s = %s;";
		}

		char *old_globals = state->globals;
		state->globals = uvsnprintf(
			fmt,
			old_globals ? old_globals : "",
			type,
			var->linkage_name,
			value
		);

		free(old_globals);
		free(value);

		return NULL;
	}

	char *old_globals = state->globals;
	state->globals = uvsnprintf(
		var->is_exported ?
			"%s\n%s %s;" :
			"%s\nstatic %s %s;",
		old_globals ? old_globals : "",
		type,
		var->linkage_name
	);

	free(old_globals);

	return gen_stmt_var_assign_c_(
		var->linkage_name, node->var_def->expr, state
	);
}

static CStmt gen_stmt_stack_var(
	const AstNode *node,
	CType type,
	SkullStateC *state
) {
	const Variable *var = node->var_def->var;

	const char *fmt = var->is_const ? "const %s %s = %s;" : "%s %s = %s;";

	CExpr expr_str = gen_expr_c(node->var_def->expr, state);
	CStmt stmt = uvsnprintf(fmt, type, var->linkage_name, expr_str);

	free(expr_str);
	return stmt;
}
