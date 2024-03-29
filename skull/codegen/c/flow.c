#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/codegen/c/core.h"
#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/shared.h"
#include "skull/semantic/symbol.h"

#include "skull/codegen/c/flow.h"

static CBlock gen_control_block_c(const AstNode *, const char *, SkullStateC *);

CStmt gen_stmt_return_c(const AstNode *node, SkullStateC *state) {
	if (!node->expr) return strdup("return;");

	CExpr expr_str = gen_expr_c(node->expr, state);
	CStmt stmt = uvsnprintf("return %s;", expr_str);

	free(expr_str);
	return stmt;
}

CStmt gen_stmt_noop_c(const AstNode *node) {
	(void)node;

	return strdup("(void)0;");
}

CStmt gen_stmt_unreachable_c(const AstNode *node) {
	(void)node;

	return strdup("while (1) {}");
}

CStmt gen_stmt_break_c(const AstNode *node) {
	(void)node;

	return strdup("break;");
}

CStmt gen_stmt_continue_c(const AstNode *node) {
	(void)node;

	return strdup("continue;");
}

CBlock gen_control_if_c(const AstNode *node, SkullStateC *state) {
	return gen_control_block_c(node, "if (%s) {\n%s\n%s}", state);
}

CBlock gen_control_elif_c(const AstNode *node, SkullStateC *state) {
	return gen_control_block_c(node, "else if (%s) {\n%s\n%s}", state);
}
CBlock gen_control_else_c(const AstNode *node, SkullStateC *state) {
	CTree tree = gen_tree_c(node->child, state);
	CBlock block = uvsnprintf("else {\n%s\n%s}", tree, get_indentation(state));

	free(tree);
	return block;
}

CBlock gen_control_while_c(const AstNode *node, SkullStateC *state) {
	return gen_control_block_c(node, "while (%s) {\n%s\n%s}", state);
}

CBlock gen_control_namespace_c(const AstNode *node, SkullStateC *state) {
	CTree tree = gen_tree_c(node->child, state);
	CBlock block = uvsnprintf(
		"\"%s\"; {\n%s\n%s}",
		node->symbol->name,
		tree,
		get_indentation(state)
	);

	free(tree);
	return block;
}

static CBlock gen_control_block_c(
	const AstNode *node,
	const char *fmt,
	SkullStateC *state
) {
	CExpr expr_str = gen_expr_c(node->expr, state);
	CTree tree = gen_tree_c(node->child, state);
	CBlock block = uvsnprintf(fmt, expr_str, tree, get_indentation(state));

	free(tree);
	free(expr_str);
	return block;
}
