#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/codegen/c/expr.h"

#include "skull/codegen/c/flow.h"

char *return_node_to_string(const AstNode *node) {
	if (!node->expr_node || !node->expr_node->expr) return strdup("return;");

	char *expr_str = expr_node_to_string(node->expr_node->expr);
	char *out = uvsnprintf("return %s;", expr_str);

	free(expr_str);
	return out;
}
