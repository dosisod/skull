#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/common/str.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/assign.h"

static CStmt gen_stmt_var_assign_c_(
	const char *,
	const AstNodeExpr *,
	SkullStateC *
);
static CStmt gen_stmt_global_var(const AstNode *, CType type, SkullStateC *);
static CStmt gen_stmt_stack_var(const AstNode *, CType type, SkullStateC *);
static void gen_stmt_constexpt_global_var(
	const Variable *,
	CType,
	SkullStateC *
);

CStmt gen_stmt_var_assign_c(const AstNode *node, SkullStateC *state) {
	return gen_stmt_var_assign_c_(
		node->var_assign->symbol->var->name,
		node->var_assign->expr,
		state
	);
}

CStmt gen_stmt_var_def_c(const AstNode *node, SkullStateC *state) {
	if (!node->var_def->symbol) return NULL;

	const Variable *var = node->var_def->symbol->var;

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
	const Variable *var = node->var_def->symbol->var;

	if (var->expr->is_const_expr) {
		gen_stmt_constexpt_global_var(var, type, state);

		return NULL;
	}

	char *old_globals = state->globals;

	if (var->is_exported) {
		state->globals = uvsnprintf(
			"%s\n%s %s __asm__(\"%s\");",
			old_globals ? old_globals : "",
			type,
			var->name,
			var->linkage_name
		);
	}
	else {
		state->globals = uvsnprintf(
			"%s\nstatic %s %s;",
			old_globals ? old_globals : "",
			type,
			var->name
		);
	}

	free(old_globals);

	return gen_stmt_var_assign_c_(var->name, node->var_def->expr, state);
}

static CStmt gen_stmt_stack_var(
	const AstNode *node,
	CType type,
	SkullStateC *state
) {
	const Variable *var = node->var_def->symbol->var;

	const char *fmt = var->is_const ? "const %s %s = %s;" : "%s %s = %s;";

	CExpr expr_str = gen_expr_c(node->var_def->expr, state);
	CStmt stmt = uvsnprintf(fmt, type, var->name, expr_str);

	free(expr_str);
	return stmt;
}

static void gen_stmt_constexpt_global_var(
	const Variable *var,
	CType type,
	SkullStateC *state
) {
	CExpr value = gen_expr_c(var->expr, state);

	const char *fmt = NULL;
	char *old_globals = state->globals;

	if (var->is_exported) {
		assert(var->is_const);
		fmt = "%s\nconst %s %s __asm__(\"%s\") = %s;";

		state->globals = uvsnprintf(
			fmt,
			old_globals ? old_globals : "",
			type,
			var->name,
			var->linkage_name,
			value
		);
	}
	else {
		if (var->is_const) fmt = "%s\nstatic const %s %s = %s;";
		else fmt = "%s\nstatic %s %s = %s;";

		state->globals = uvsnprintf(
			fmt,
			old_globals ? old_globals : "",
			type,
			var->name,
			value
		);
	}

	free(old_globals);
	free(value);
}
