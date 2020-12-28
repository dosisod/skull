# skull/parse/ast_node

```c
#define ATTR(from, node, prop) ((from *)(node)->attr)->prop
```

> Get the attribute/property `prop` from `node`, assuming it is of type `from`.

```c
#define MAKE_ATTR(from, node, ...) \
	from *attr; \
	attr = Malloc(sizeof *attr); \
	*attr = (from){ \
		__VA_ARGS__ \
	}; \
	(node)->attr = attr
```

> Create a new attribute struct of type `from`, assign to `node`, with data passed from `...`.

```c
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
}
```

> An `AstNode` abstractly stores data about parsed code.
> \
> Each `AstNode` must have a `node_type`, which represents what tokens a node might contain.
> \
> An `AstNode` points to the starting `token`, and the ending token, `token_end`.
> \
> Adjacent `AstNode`s are accessed via `next` and `last`.
> \
> `child` and `parent` are used to access the child and parent node of a given node.
> \
> Certain `node_type`s may chose to store arbitrary data in `attr` to save time re-parsing.

```c
typedef struct {
	// these probably should be turned into bitflags
	_Bool is_implicit;
	_Bool is_const;
}
```

> Used to store special data about `AST_NODE_VAR_DEF` nodes.

```c
typedef struct {
	const Type *param_types;
	char32_t *param_names;

	const Type *return_type;

	_Bool is_external;
	_Bool is_export;
}
```

> Used to store special data about `AST_NODE_FUNCTION_PROTO` nodes.

```c
typedef struct {
	const Token *param;
}
```

> Store special data for `AST_NODE_FUNCTION` nodes.

```c
typedef struct {
	const Token *lhs;
	const Token *rhs;
}
```

> Store special data about operator related nodes (`AST_NODE_ADD`, etc).

```c
typedef struct {
	const Token *lhs;
	TokenType oper;
	const Token *rhs;
}
```

> Store data about a conditional expression.

```c
#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)
```

> Compare a variable number of token types stored in `...` agains each successive token in `token`.

```c
AstNode *make_ast_tree(const char32_t *const code)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl, Token **token_last)
```

> Internal AST tree generator.

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

> Check each token's type starting at `token`, checking against the corresponding token type specified in `...`

```c
__attribute__((pure)) bool is_value(const Token *const token)
```

> Return whether `token` represents a constant literal, or an identifier.

