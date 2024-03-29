#pragma once

#include <stdint.h>

#include "skull/parse/token.h"
#include "skull/semantic/types.h"

typedef enum {
	AST_NODE_UNKNOWN,

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
	AST_NODE_IMPORT,
	AST_NODE_NAMESPACE,

	AST_NODE_COMMENT,
	AST_NODE_NOOP,
	AST_NODE_BREAK,
	AST_NODE_CONTINUE
} NodeType;

typedef struct AstNode AstNode;
typedef struct AstNodeVarDef AstNodeVarDef;
typedef struct AstNodeVarAssign AstNodeVarAssign;
typedef struct AstNodeFunctionProto AstNodeFunctionProto;
typedef struct AstNodeFunctionCall AstNodeFunctionCall;
typedef struct AstNodeExpr AstNodeExpr;
typedef struct Symbol Symbol;

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
		AstNodeVarAssign *var_assign;
		AstNodeFunctionProto *func_proto;
		AstNodeExpr *expr;
		_Bool is_void_return;
		Symbol *symbol;
	};
} AstNode;

/*
Used to store special data about `AST_NODE_VAR_DEF` nodes.
*/
typedef struct AstNodeVarDef {
	const Token *name_tok;
	AstNodeExpr *expr;
	Symbol *symbol;
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	_Bool is_exported: 1;
} AstNodeVarDef;

/*
Used to store special data about `AST_NODE_VAR_ASSIGN` nodes.
*/
typedef struct AstNodeVarAssign {
	AstNodeExpr *expr;
	Symbol *symbol;
} AstNodeVarAssign;

/*
Store information about a function's parameters (name, type, etc)
*/
typedef struct AstNodeFunctionParam {
	Token *type_name;
	char32_t *param_name;
	Symbol *symbol;
	Location *location; // TODO(dosisod): remove when location is in symbol
} AstNodeFunctionParam;

/*
Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.
*/
typedef struct AstNodeFunctionProto {
	const Token *name_tok;

	Token *return_type_token;

	unsigned short num_params;

	Symbol *symbol;

	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam *params[];
} AstNodeFunctionProto;

/*
Used to store special data about function call.
*/
typedef struct AstNodeFunctionCall {
	const Token *func_name_tok;
	AstNode *params;
	unsigned short num_values;
	Symbol *symbol;
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
	EXPR_FUNC,
	EXPR_REF,
	EXPR_DEREF
} ExprType;

/*
Store special data about operator related nodes.
*/
typedef struct AstNodeExpr {
	union {
		const Token *tok;
		AstNodeExpr *expr;
		AstNodeFunctionCall *func_call;
	} lhs;

	Symbol *symbol;

	union {
		int64_t _int;
		double _float;
		_Bool _bool;
		char32_t rune;
		char *str;
	} value;
	const Type *type;

	AstNodeExpr *rhs;

	ExprType oper;

	_Bool is_const_expr;
} AstNodeExpr;

/*
Makes an empty AstNode.
*/
#define make_ast_node() Calloc(1, sizeof(AstNode))

AstNode *parse_ast_tree(Token *);
void free_ast_tree(AstNode *);

const Location *find_expr_node_location(const AstNodeExpr *);
const Token *find_expr_node_token(const AstNodeExpr *);
