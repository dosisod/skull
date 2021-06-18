#pragma once

#include "skull/parse/token.h"

typedef enum {
	AST_NODE_UNKNOWN,

	AST_NODE_TYPE_ALIAS,

	AST_NODE_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_FUNCTION_PROTO,

	AST_NODE_EXPR,

	AST_NODE_RETURN,
	AST_NODE_UNREACHABLE,
	AST_NODE_IF,
	AST_NODE_ELIF,
	AST_NODE_ELSE,
	AST_NODE_WHILE,

	AST_NODE_COMMENT,
	AST_NODE_NOOP
} NodeType;

typedef struct AstNode AstNode;
typedef struct AstNodeVarDef AstNodeVarDef;
typedef struct AstNodeFunctionProto AstNodeFunctionProto;
typedef struct AstNodeFunctionCall AstNodeFunctionCall;
typedef struct AstNodeExpr AstNodeExpr;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct Variable Variable;

/*
An `AstNode` abstractly stores data about parsed code.

Each `AstNode` must have a `type`, which represents what tokens a node might
contain.

An `AstNode` points to the starting `token`, and the ending token, `token_end`.

Adjacent `AstNode`s are accessed via `next` and `last`.

`child` and `parent` are used to access the child and parent node of a given
node.
*/
typedef struct AstNode {
	NodeType type;

	Token *token;
	Token *token_end;

	AstNode *next;
	AstNode *last;

	AstNode *child;
	AstNode *parent;

	// used to store arbitrary data associated with a certain node type
	union {
		AstNodeVarDef *var_def;
		AstNodeFunctionProto *func_proto;
		AstNodeExpr *expr;
		AstNode *expr_node;
		_Bool is_void_return;
	};
} AstNode;

/*
Used to store special data about `AST_NODE_VAR_DEF` nodes.
*/
typedef struct AstNodeVarDef {
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	_Bool is_exported: 1;
	const Token *name_tok;
	AstNode *expr_node;
} AstNodeVarDef;

/*
Store information about a function's parameters (name, type, etc)
*/
typedef struct AstNodeFunctionParam {
	char *type_name;
	char32_t *param_name;
	Variable *var;
} AstNodeFunctionParam;

/*
Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.
*/
typedef struct AstNodeFunctionProto {
	const Token *name_tok;

	char *return_type_name;

	unsigned short num_params;

	FunctionDeclaration *func;

	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam *params[];
} AstNodeFunctionProto;

/*
Used to store special data about function call.
*/
typedef struct AstNodeFunctionCall {
	const Token *func_name_tok;
	const AstNode *params;
	unsigned short num_values;
} AstNodeFunctionCall;

typedef enum {
	EXPR_UNKNOWN,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_UNARY_NEG,
	EXPR_MULT,
	EXPR_DIV,
	EXPR_MOD,
	EXPR_LSHIFT,
	EXPR_RSHIFT,
	EXPR_POW,
	EXPR_NOT,
	EXPR_IS,
	EXPR_ISNT,
	EXPR_LESS_THAN,
	EXPR_GTR_THAN,
	EXPR_LESS_THAN_EQ,
	EXPR_GTR_THAN_EQ,
	EXPR_AND,
	EXPR_OR,
	EXPR_XOR,
	EXPR_IDENTIFIER,
	EXPR_CONST,
	EXPR_FUNC
} ExprType;

/*
Store special data about operator related nodes.
*/
typedef struct AstNodeExpr {
	union {
		const Token *tok;
		const AstNodeExpr *expr;
	} lhs;

	union {
		const Token *tok;
		const AstNodeExpr *expr;
	} rhs;

	ExprType oper;

	// only for use in function expressions
	AstNodeFunctionCall *func_call;
} AstNodeExpr;

/*
Makes an empty AstNode.
*/
#define make_ast_node() Calloc(1, sizeof(AstNode))

void push_ast_node(Token *const, Token *, NodeType, AstNode **);

AstNode *parse_ast_tree(const char32_t *const);
void free_ast_tree(AstNode *);
