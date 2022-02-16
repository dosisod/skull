# skull/parse/ast_node

```c
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
	};
}
```

> An `AstNode` abstractly stores data about parsed code.
> \
> Each `AstNode` must have a `type`, which represents what tokens a node might
> contain.
> \
> An `AstNode` points to the starting `token`, and the ending token, `token_end`.
> \
> Adjacent `AstNode`s are accessed via `next` and `last`.
> \
> `child` and `parent` are used to access the child and parent node of a given
> node.

```c
typedef struct AstNodeVarDef {
	const Token *name_tok;
	AstNodeExpr *expr;
	Symbol *symbol;
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	_Bool is_exported: 1;
}
```

> Used to store special data about `AST_NODE_VAR_DEF` nodes.

```c
typedef struct AstNodeVarAssign {
	AstNodeExpr *expr;
	Symbol *symbol;
}
```

> Used to store special data about `AST_NODE_VAR_ASSIGN` nodes.

```c
typedef struct AstNodeFunctionParam {
	Token *type_name;
	char32_t *param_name;
	Symbol *symbol;
	Location *location; // TODO(dosisod): remove when location is in symbol
}
```

> Store information about a function's parameters (name, type, etc)

```c
typedef struct AstNodeFunctionProto {
	const Token *name_tok;

	Token *return_type_token;

	unsigned short num_params;

	Symbol *symbol;

	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam *params[];
}
```

> Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.

```c
typedef struct AstNodeFunctionCall {
	const Token *func_name_tok;
	AstNode *params;
	unsigned short num_values;
	Symbol *symbol;
}
```

> Used to store special data about function call.

```c
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
}
```

> Store special data about operator related nodes.

```c
#define make_ast_node() Calloc(1, sizeof(AstNode))
```

> Makes an empty AstNode.

```c
AstNode *parse_ast_tree(Token *token)
```

> Makes an AST (abstract syntax tree) from a list of tokens.

```c
static Token *is_potential_type(Token *token)
```

> Check whether the tokens starting at `token` is a potential type. Return
> last token of type if valid, otherwise `NULL`.

```c
static ExprType token_type_to_binary_expr_type(TokenType type)
```

> Convert a `TokenType` to an `ExprType` in a binary-expr context

```c
static AstNode *parse_ast_tree_(ParserCtx *ctx)
```

> Internal AST tree generator.
> \
> Do not set the `node` or `head` fields in `ctx`, they will be overridden.

```c
static bool parse_ast_node(ParserCtx *ctx)
```

> Parse a single AST node.
> \
> Return `true` if a terminating token was reached (closing bracket).

```c
static void parse_ast_sub_tree_(ParserCtx *ctx)
```

> Start parsing a AST sub tree.

```c
static AstNodeExpr *parse_expression(ParserCtx *ctx)
```

> Try and generate AST node for expression.
> \
> Returns node if one was added, NULL otherwise.

```c
static AstNodeExpr *parse_root_expr(ParserCtx *ctx)
```

> Parse root expression from context `ctx`. A root expression is an expression
> which has no association, and as such, has the highest precedence.

```c
static AstNodeExpr *parse_single_token_expr(ParserCtx *ctx)
```

> Parse a single token expression (constant or variable) from `ctx`.

```c
static AstNodeExpr *parse_func_call(ParserCtx *ctx)
```

> Try and generate AST node for a function call.
> \
> Returns true if a node was added, false otherwise.

```c
static AstNode *push_ast_node(ParserCtx *ctx, Token *last, NodeType node_type)
```

> Push a new AST node to `node` with type `node_type`. Return created node.

```c
static void free_expr_node(AstNodeExpr *expr)
```

> Free an expression nodes and all its sub-expressions.

```c
void free_ast_tree(AstNode *node)
```

> Frees an AST tree.

```c
static __attribute__((pure)) bool is_single_token_expr(TokenType token_type)
```

> Return whether `token_type` represents a constant literal, or an identifier.

```c
void print_ast_tree(const AstNode *node)
```

> Print AST tree to screen (for debugging).

```c
static void print_ast_tree_(const AstNode *node, unsigned indent_lvl)
```

> The actual `print_ast_tree` function.
> \
> Print `node` and all of its properties (including tokens).
> Indent more depending on the value of `indent_lvl`.

