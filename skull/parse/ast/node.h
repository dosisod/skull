#pragma once

#include "skull/parse/tokenize.h"

#define TOKEN_ANY_NON_BRACKET_TOKEN (TOKEN_END + 1)

#define ATTR(from, node, prop) ((from *)(node)->attr)->prop

typedef enum {
	AST_NODE_UNKNOWN,

	AST_NODE_IDENTIFIER,

	AST_NODE_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_IMPORT,

	AST_NODE_ADD_VAR,
	AST_NODE_SUB_VAR,
	AST_NODE_MULT_VAR,
	AST_NODE_DIV_VAR,

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

typedef enum {
	RULE_OPTIONAL = 1
} ComboRule;

typedef struct Combo Combo;

typedef struct Combo {
	Combo *combo;
	unsigned tok;
	ComboRule rule;
} Combo;

Token *ast_token_cmp(Token *, Combo *, _Bool *);
void push_ast_node(Token *, Token **, NodeType, AstNode **);

AstNode *make_ast_tree(const char32_t *, const char32_t **);
AstNode *make_ast_tree_(Token *, const char32_t **, unsigned);
void free_ast_tree(AstNode *);