# skull/parse/ast/node

```c
AstNode *make_ast_tree(const char32_t *const code)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl)
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

