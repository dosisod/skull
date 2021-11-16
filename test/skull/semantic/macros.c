#include "skull/parse/ast_node.h"

#include "./macros.h"

void AST_NODE_FUNC_ADD_PARAM(AstNode *node, AstNode *param) {
	AstNodeFunctionCall *func_call = node->expr->lhs.func_call;
	func_call->num_values++;

	AstNode *params = (AstNode *)func_call->params;
	if (!params) {
		func_call->params = param;
		return;
	}

	while (params && params->next) params = params->next;

	params->next = param;
}
