#include <stddef.h>
#include <stdlib.h>

#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/types.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/assign.h"

char *var_assign_to_string(const AstNode *node) {
	char *expr_str = expr_node_to_string(node->var_assign->expr_node->expr);

	char *stmt = uvsnprintf("%s = %s;", node->var_assign->var->name, expr_str);
	free(expr_str);

	return stmt;
}

char *var_def_to_string(const AstNode *node) {
	const Variable *var = node->var_def->var;

	const char *type = skull_type_to_c_type(var->type);
	char *expr_str = expr_node_to_string(node->var_def->expr_node->expr);

	const char *fmt = var->is_const ? "const %s %s = %s;" : "%s %s = %s;";
	char *out = uvsnprintf(fmt, type, var->name, expr_str);

	free(expr_str);
	return out;
}
