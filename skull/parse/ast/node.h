#pragma once

#include "skull/parse/tokenize.h"

#define ATTR(from, node, prop) ((from *)(node)->attr)->prop

typedef enum {
	AST_NODE_UNKNOWN,

	AST_NODE_IDENTIFIER,

	AST_NODE_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_EXTERNAL,

	AST_NODE_FUNCTION,
	AST_NODE_FUNCTION_PROTO,

	AST_NODE_ADD_CONSTS,
	AST_NODE_SUB_CONSTS,
	AST_NODE_MULT_CONSTS,
	AST_NODE_DIV_CONSTS,

	AST_NODE_RETURN,

	AST_NODE_IF,

	AST_NODE_INT_CONST,
	AST_NODE_FLOAT_CONST,
	AST_NODE_BOOL_CONST,
	AST_NODE_RUNE_CONST,
	AST_NODE_STR_CONST,
	AST_NODE_TYPE_CONST,

	AST_NODE_COMMENT
} NodeType;

typedef struct AstNode AstNode;

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

typedef struct {
	// these probably should be turned into bitflags
	_Bool is_implicit;
	_Bool is_const;
} AstNodeVarDef;

AstNode *make_ast_node(void);

void push_ast_node(Token *const, Token **, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *const);
AstNode *make_ast_tree_(Token *, unsigned);
void free_ast_tree(AstNode *);
void free_ast_tree_(AstNode *);

_Bool is_const_literal(Token *);

#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)

_Bool ast_token_cmp(Token *, ...);