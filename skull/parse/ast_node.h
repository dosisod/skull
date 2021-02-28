#pragma once

#include "skull/eval/types/types.h"
#include "skull/parse/token.h"

/*
Get the attribute/property `prop` from `node`, assuming it is of type `from`.
*/
#define ATTR(from, node, prop) ((from *)(node)->attr)->prop

typedef enum {
	AST_NODE_UNKNOWN,

	AST_NODE_IDENTIFIER,

	AST_NODE_TYPE_ALIAS,

	AST_NODE_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_FUNCTION,
	AST_NODE_FUNCTION_PROTO,

	AST_NODE_OPER_EXPR,
	AST_NODE_BOOL_EXPR,

	AST_NODE_RETURN,
	AST_NODE_UNREACHABLE,
	AST_NODE_IF,
	AST_NODE_ELIF,
	AST_NODE_ELSE,
	AST_NODE_WHILE,

	AST_NODE_CONST,

	AST_NODE_COMMENT
} NodeType;

typedef struct AstNode AstNode;

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
	void *attr;
} AstNode;

/*
Used to store special data about `AST_NODE_VAR_DEF` nodes.
*/
typedef struct {
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	const Token *name_tok;
} AstNodeVarDef;

/*
Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.
*/
typedef struct {
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
typedef struct {
	const Token *func_name_tok;
	unsigned short num_values;
} AstNodeFunctionCall;

/*
Store special data about operator related nodes.
*/
typedef struct {
	const Token *lhs;
	TokenType oper;
	const Token *rhs;
} AstNodeOper;

AstNode *make_ast_node(void);

void push_ast_node(Token *const, Token **, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *const);
void free_ast_tree(AstNode *);
