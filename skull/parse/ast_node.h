#pragma once

#include "skull/compiler/types/types.h"
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

	AST_NODE_COMMENT
} NodeType;

typedef struct AstNode AstNode;
typedef struct AstNodeVarDef AstNodeVarDef;
typedef struct AstNodeFunctionProto AstNodeFunctionProto;
typedef struct AstNodeFunctionCall AstNodeFunctionCall;
typedef struct AstNodeExpr AstNodeExpr;

/*
An `AstNode` abstractly stores data about parsed code.

Each `AstNode` must have a `type`, which represents what tokens a node might
contain.

An `AstNode` points to the starting `token`, and the ending token, `token_end`.

Adjacent `AstNode`s are accessed via `next` and `last`.

`child` and `parent` are used to access the child and parent node of a given
node.

Certain `node_type`s may chose to store arbitrary data in `attr` to save time
re-parsing.
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
	} attr;
} AstNode;

/*
Used to store special data about `AST_NODE_VAR_DEF` nodes.
*/
typedef struct AstNodeVarDef {
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	const Token *name_tok;
} AstNodeVarDef;

/*
Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.
*/
typedef struct AstNodeFunctionProto {
	const Token *name_tok;

	char **param_type_names;
	char32_t **param_names;

	char *return_type_name;

	unsigned short num_params;

	_Bool is_external : 1;
	_Bool is_export : 1;
} AstNodeFunctionProto;

/*
Used to store special data about function call.
*/
typedef struct AstNodeFunctionCall {
	const Token *func_name_tok;
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
	const Token *lhs;
	const Token *rhs;
	ExprType oper;

	// only for use in function expressions
	AstNodeFunctionCall *func_call;
} AstNodeExpr;

/*
Makes an empty AstNode.
*/
#define make_ast_node() Calloc(1, sizeof(AstNode))

void push_ast_node(Token *const, Token *, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *const);
void free_ast_tree(AstNode *);
