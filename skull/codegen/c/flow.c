#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/codegen/c/expr.h"

#include "skull/codegen/c/flow.h"

CStmt gen_stmt_return_c(const AstNode *node) {
	if (!node->expr_node || !node->expr_node->expr) return strdup("return;");

	CExpr expr_str = gen_expr_c(node->expr_node->expr);
	CStmt stmt = uvsnprintf("return %s;", expr_str);

	free(expr_str);
	return stmt;
}

CStmt gen_stmt_noop_c(const AstNode *node) {
	(void)node;

	return strdup("(void)0;");
}

CBlock gen_control_if_c(const AstNode *node) {
	CExpr expr_str = gen_expr_c(node->expr_node->expr);

	CBlock block = uvsnprintf("if (%s) {}", expr_str);

	free(expr_str);
	return block;
}
