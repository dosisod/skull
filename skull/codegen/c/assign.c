#include <stddef.h>
#include <stdlib.h>

#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/types.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/assign.h"

CStmt var_assign_to_string(const AstNode *node) {
	CExpr expr_str = expr_node_to_string(node->var_assign->expr_node->expr);

	CStmt stmt = uvsnprintf("%s = %s;", node->var_assign->var->name, expr_str);
	free(expr_str);

	return stmt;
}

CStmt var_def_to_string(const AstNode *node) {
	const Variable *var = node->var_def->var;

	CType type = skull_type_to_c_type(var->type);
	CExpr expr_str = expr_node_to_string(node->var_def->expr_node->expr);

	const char *fmt = var->is_const ? "const %s %s = %s;" : "%s %s = %s;";
	CStmt stmt = uvsnprintf(fmt, type, var->name, expr_str);

	free(expr_str);
	return stmt;
}
