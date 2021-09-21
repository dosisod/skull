#pragma once

#define AST_CONST_EXPR(token) \
	&(AstNodeExpr){ \
		.lhs = { \
			.tok = (token) \
		}, \
		.oper = EXPR_CONST \
	}

#define AST_BINARY_EXPR(_lhs, _oper, _rhs) \
	&(AstNodeExpr){ \
		.lhs = { .expr = (_lhs) }, \
		.rhs = (_rhs), \
		.oper = (_oper) \
	}

#define AST_SIMPLE_EXPR(_token) \
	&(AstNode){ \
		.type = AST_NODE_EXPR, \
		.token = (_token), \
		.expr = AST_CONST_EXPR(_token) \
	}

#define AST_SIMPLE_BINARY_EXPR(_token, _oper) \
	&(AstNode){ \
		.type = AST_NODE_EXPR, \
		.token = (_token), \
		.expr = AST_BINARY_EXPR( \
			AST_CONST_EXPR(_token), \
			(_oper), \
			AST_CONST_EXPR((_token)->next->next) \
		) \
	}

#define AST_NO_ARG_FUNC_EXPR(_token) \
	&(AstNodeExpr){ \
		.lhs = { \
			.func_call = &(AstNodeFunctionCall){ \
				.func_name_tok = (_token) \
			} \
		}, \
		.oper = EXPR_FUNC \
	}

#define AST_IDENT_EXPR(_token) \
	&(AstNodeExpr){ \
		.lhs = { .tok = (_token) }, \
		.oper = EXPR_IDENTIFIER \
	}

#define AST_SIMPLE_UNARY_EXPR(_token, _oper) \
	&(AstNode){ \
		.type = AST_NODE_EXPR, \
		.token = (_token), \
		.expr = AST_BINARY_EXPR( \
			NULL, \
			(_oper), \
			AST_CONST_EXPR((_token)->next) \
		) \
	}
