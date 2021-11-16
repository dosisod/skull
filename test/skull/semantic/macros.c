#include "skull/parse/ast_node.h"

#include "./macros.h"

void AST_NODE_FUNC_ADD_PARAM(AstNodeExpr *expr, AstNode *param) {
	AstNodeFunctionCall *func_call = expr->lhs.func_call;
	func_call->num_values++;
	func_call->func_decl->num_params++;

	AstNode *params = (AstNode *)func_call->params;
	if (!params) {
		func_call->params = param;
		return;
	}

	while (params && params->next) params = params->next;

	params->next = param;
}
