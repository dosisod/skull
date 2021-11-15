#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/codegen/c/expr.h"

#include "skull/codegen/c/flow.h"

CStmt return_node_to_string(const AstNode *node) {
	if (!node->expr_node || !node->expr_node->expr) return strdup("return;");

	CExpr expr_str = expr_node_to_string(node->expr_node->expr);
	CStmt stmt = uvsnprintf("return %s;", expr_str);

	free(expr_str);
	return stmt;
}

CStmt noop_to_string(const AstNode *node) {
	(void)node;

	return strdup("(void)0;");
}

CBlock if_to_string(const AstNode *node) {
	CExpr expr_str = expr_node_to_string(node->expr_node->expr);

	CBlock block = uvsnprintf("if (%s) {}", expr_str);

	free(expr_str);
	return block;
}
