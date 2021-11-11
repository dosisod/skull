#pragma once

#define AST_NODE_CONST_EXPR(token) \
	&(AstNodeExpr){ \
		.lhs = { \
			.tok = (token) \
		}, \
		.oper = EXPR_CONST \
	}

#define AST_NODE_EXPR(_token, _expr) \
	&(AstNode){ \
		.type = AST_NODE_EXPR, \
		.token = (_token), \
		.expr = (_expr) \
	}

#define AST_NODE_BINARY_EXPR(_lhs, _oper, _rhs) \
	&(AstNodeExpr){ \
		.lhs = { .expr = (_lhs) }, \
		.rhs = (_rhs), \
		.oper = (_oper) \
	}

#define AST_SIMPLE_EXPR(_token) \
	AST_NODE_EXPR((_token), AST_NODE_CONST_EXPR(_token))

#define AST_SIMPLE_BINARY_EXPR(_token, _oper) \
	AST_NODE_EXPR( \
		(_token), \
		AST_NODE_BINARY_EXPR( \
			AST_NODE_CONST_EXPR(_token), \
			(_oper), \
			AST_NODE_CONST_EXPR((_token)->next->next) \
		) \
	)

#define AST_NO_ARG_FUNC_EXPR(_token) \
	&(AstNodeExpr){ \
		.lhs = { \
			.func_call = &(AstNodeFunctionCall){ \
				.func_name_tok = (_token) \
			} \
		}, \
		.oper = EXPR_FUNC \
	}

#define AST_NODE_IDENT_EXPR(_token) \
	&(AstNodeExpr){ \
		.lhs = { .tok = (_token) }, \
		.oper = EXPR_IDENTIFIER \
	}

#define AST_SIMPLE_UNARY_EXPR(_token, _oper) \
	AST_NODE_EXPR( \
		(_token), \
		AST_NODE_BINARY_EXPR( \
			NULL, \
			(_oper), \
			AST_NODE_CONST_EXPR((_token)->next) \
		) \
	)

#define AST_NODE_VAR_ASSIGN(_token, _expr_node) \
	&(AstNode){ \
		.type = AST_NODE_VAR_ASSIGN, \
		.token = (_token), \
		.var_assign = &(AstNodeVarAssign){ \
			.expr_node = (_expr_node), \
			.var = NULL \
		} \
	}

#define AST_NODE_VAR_DEF(_token, _expr_node, _is_implicit) \
	&(AstNode){ \
		.type = AST_NODE_VAR_DEF, \
		.token = (_token), \
		.token_end = (_token) ? ((_is_implicit) ? (_token)->next : (_token)->next->next) : NULL, \
		.var_def = &(AstNodeVarDef){ \
			.name_tok = (_token), \
			.expr_node = (_expr_node), \
			.var = NULL, \
			.is_implicit = (_is_implicit), \
			.is_const = true \
		} \
	}

#define AST_NODE_NO_ARGS_FUNC_DECL(_token, _is_external, _is_export) \
	&(AstNode){ \
		.type = AST_NODE_FUNCTION_PROTO, \
		.token = ((_is_export) || (_is_external)) ? (_token)->next : (_token), \
		.func_proto = &(AstNodeFunctionProto){ \
			.name_tok = ((_is_export) || (_is_external)) ? \
				(_token)->next : \
				(_token), \
			.return_type_name = NULL, \
			.is_external = (_is_external), \
			.is_export = (_is_export), \
		} \
	}

#define AST_NODE_NOOP() \
	&(AstNode){ \
		.type = AST_NODE_NOOP, \
		.token = NULL \
	}

#define AST_NODE_ELSE(_token, _child) \
	&(AstNode){ \
		.type = AST_NODE_ELSE, \
		.token = (_token), \
		.child = (_child), \
		.parent = NULL \
	}

#define AST_NODE_COMMENT(_token) \
	&(AstNode){ \
		.type = AST_NODE_COMMENT, \
		.token = (_token) \
	}

#define AST_NODE_RETURN(_token, _expr) \
	&(AstNode){ \
		.type = AST_NODE_RETURN, \
		.token = (_token), \
		.expr_node = (_expr), \
	}

#define AST_NODE_IF(_token, _cond, _child) \
	&(AstNode){ \
		.type = AST_NODE_IF, \
		.token = (_token), \
		.child = (_child), \
		.expr_node = (_cond), \
		.parent = NULL \
	}

#define AST_NODE_ELIF(_token, _cond, _child) \
	&(AstNode){ \
		.type = AST_NODE_ELIF, \
		.token = (_token), \
		.child = (_child), \
		.expr_node = (_cond), \
		.parent = NULL \
	}
