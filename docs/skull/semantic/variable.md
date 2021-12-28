# skull/semantic/variable

```c
typedef struct Variable {
	Type type;
	char *name;
	_Bool is_const : 1;
	_Bool is_const_lit : 1;
	_Bool is_global : 1;
	_Bool was_reassigned : 1;
	_Bool was_read : 1;
	_Bool implicitly_typed : 1;
	_Bool is_exported : 1;
	_Bool is_defined : 1;

	Location location;

	void *ref;
	AstNodeExpr *expr;
}
```

> `type` is the Skull type of the variable.
> \
> `name` is the name of the variable.
> \
> `is_const` stores whether the variable is const or not.
> \
> `is_const_lit` stores whether the variable is const literal.
> \
> `is_global` stores whether the variable was declared at the top level of a
> module.
> \
> `was_reassigned` stores whether a (mutable) variable was reassigned.
> \
> `was_read` stores whether a variable was read from or not.
> \
> `implicitly_typed` is true when variable type is auto-deduced.
> \
> `is_exported` is true when the variable is marked as exported.
> \
> `is_defined` is set once a variable is done being assigned (used to detect
> self-referencing the variable currently being assignment to).
> \
> `location` stores the location where the variable was first defined.
> \
> `ref` stores arbitrary data, for use in the backend.

```c
Variable *make_variable(Type type, const char32_t *const name, bool is_const)
```

> Make a variable called `name` with type `type`, and make it const if
> `is_const` is true.
> \
> Returns `NULL` if var cannot be created, else pointer to created var.

```c
void free_variable(Variable *var)
```

> Free variable `var`.

```c
void variable_disable_warnings(Variable *var)
```

> Disable warnings on variable `var`.

