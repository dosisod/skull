#pragma once

#include "skull/eval/types/types.h"
#include "skull/parse/tokenize.h"

#define ATTR(from, node, prop) ((from *)(node)->attr)->prop

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
	const char32_t *param_names;

	const Type *return_type;

	_Bool is_external;
} AstNodeFunctionProto;

AstNode *make_ast_node(void);

void push_ast_node(Token *const, Token **, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *const);
AstNode *make_ast_tree_(Token *, unsigned);
void free_ast_tree(AstNode *);
void free_ast_tree_(AstNode *);

_Bool is_const_literal(Token *);

#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)

_Bool ast_token_cmp(Token *, ...);