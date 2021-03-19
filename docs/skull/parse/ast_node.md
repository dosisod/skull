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
		AstNodeFunctionProto *func_proto;
		AstNodeFunctionCall *func_call;
		AstNodeExpr *expr;
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
> \
> Certain `node_type`s may chose to store arbitrary data in `attr` to save time
> re-parsing.

```c
typedef struct AstNodeVarDef {
	_Bool is_implicit : 1;
	_Bool is_const : 1;
	const Token *name_tok;
}
```

> Used to store special data about `AST_NODE_VAR_DEF` nodes.

```c
typedef struct AstNodeFunctionProto {
	const Token *name_tok;

	char **param_type_names;
	char32_t **param_names;

	char *return_type_name;

	unsigned short num_params;

	_Bool is_external : 1;
	_Bool is_export : 1;
}
```

> Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.

```c
typedef struct AstNodeFunctionCall {
	const Token *func_name_tok;
	unsigned short num_values;
}
```

> Used to store special data about function call.

```c
typedef struct AstNodeExpr {
	const Token *lhs;
	const Token *rhs;
	ExprType oper;
}
```

> Store special data about operator related nodes.

```c
#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)
```

> Compare a variable number of token types stored in `...` agains each
> successive token in `token`.

```c
AstNode *make_ast_tree(const char32_t *const code)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl, Token **token_last)
```

> Internal AST tree generator.

```c
bool try_gen_expression(Token **_token, Token **last, AstNode **node)
```

> Try and generate AST node for expression.
> \
> Returns true if a node was added, false otherwise.

```c
void gen_func_call(Token **_token, Token **last, AstNode **node)
```

> Try and generate AST node for a function call.
> \
> Returns true if a node was added, false otherwise.

```c
void push_ast_node(Token *const token, Token **last, NodeType node_type, AstNode **node)
```

> Push a new AST node to `node` with type `node_type`

```c
AstNode *make_ast_node(void)
```

> Makes an AstNode with default values

```c
void free_ast_tree(AstNode *node)
```

> Frees an AST tree.

```c
void free_ast_tree_(AstNode *node)
```

> Internal AST freeing function, dont call directly.

```c
bool ast_token_cmp(Token *token, ...)
```

> Check each token's type starting at `token`, checking against the
> corresponding token type specified in `...`

```c
__attribute__((pure)) bool is_value(const Token *const token)
```

> Return whether `token` represents a constant literal, or an identifier.

```c
void print_ast_tree(const AstNode *node)
```

> Print AST tree to screen (for debugging).

