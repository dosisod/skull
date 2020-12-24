#pragma once

#include "skull/eval/types/types.h"
#include "skull/parse/token.h"

/*
Get the attribute/property `prop` from `node`, assuming it is of type `from`.
*/
#define ATTR(from, node, prop) ((from *)(node)->attr)->prop

/*
Create a new attribute struct of type `from`, assign to `node`, with data passed from `...`.
*/
#define MAKE_ATTR(from, node, ...) \
	from *attr; \
	attr = malloc(sizeof *attr); \
	DIE_IF_MALLOC_FAILS(attr); \
	*attr = (from){ \
		__VA_ARGS__ \
	}; \
	(node)->attr = attr

typedef enum {
	AST_NODE_UNKNOWN,

	AST_NODE_IDENTIFIER,

	AST_NODE_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_FUNCTION,
	AST_NODE_FUNCTION_PROTO,

	AST_NODE_ADD,
	AST_NODE_SUB,
	AST_NODE_MULT,
	AST_NODE_DIV,

	AST_NODE_RETURN,
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

Each `AstNode` must have a `node_type`, which represents what tokens a node might contain.

An `AstNode` points to the starting `token`, and the ending token, `token_end`.

Adjacent `AstNode`s are accessed via `next` and `last`.

`child` and `parent` are used to access the child and parent node of a given node.

Certain `node_type`s may chose to store arbitrary data in `attr` to save time re-parsing.
*/
typedef struct AstNode {
	NodeType node_type;

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
	// these probably should be turned into bitflags
	_Bool is_implicit;
	_Bool is_const;
} AstNodeVarDef;

/*
Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.
*/
typedef struct {
	const Type *param_types;
	char32_t *param_names;

	const Type *return_type;

	_Bool is_external;
	_Bool is_export;
} AstNodeFunctionProto;

/*
Store special data for `AST_NODE_FUNCTION` nodes.
*/
typedef struct {
	Token *param;
} AstNodeFunction;

/*
Store special data about operator related nodes (`AST_NODE_ADD`, etc).
*/
typedef struct {
	Token *lhs;
	Token *rhs;
} AstNodeOper;

/*
Store data about a conditional expression.
*/
typedef struct {
	Token *rhs;
	TokenType oper;
} AstNodeBoolExpr;

AstNode *make_ast_node(void);

void push_ast_node(Token *const, Token **, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *const);
AstNode *make_ast_tree_(Token *, unsigned, Token **);
void free_ast_tree(AstNode *);
void free_ast_tree_(AstNode *);

_Bool is_const_literal(Token *);

/*
Compare a variable number of token types stored in `...` agains each successive token in `token`.
*/
#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)

_Bool ast_token_cmp(Token *, ...);