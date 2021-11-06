#include <stddef.h>
#include <stdlib.h>

#include "skull/codegen/c/expr.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/assign.h"

char *var_assign_to_string(const AstNode *node) {
	char *expr_str = expr_node_to_string(node->var_assign->expr_node->expr);

	char *stmt = uvsnprintf("%s = %s;", node->var_assign->var->name, expr_str);
	free(expr_str);

	return stmt;
}
