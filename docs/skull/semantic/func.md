# skull/semantic/func

```c
typedef struct FunctionDeclaration {
	char *name;
	Location location;

	void *ref;
	void *type;

	unsigned short num_params;
	Type **param_types;
	char32_t **param_names;

	Type *return_type;

	_Bool was_called : 1;
	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam **params;
}
```

> Stores a function declaration.
> \
> `name` is the name of the declared function.
> \
> `location` is the location of the function name.
> \
> `ref` is for storing arbitrary information, for use in the backend
> \
> `type` same as `ref`.
> \
> `num_params` stores the number of params a function can take.
> \
> `param_types` is the Skull type for each function param.
> \
> `return_type` is the Skull type that the function returns.

```c
FunctionDeclaration *find_func_by_name(const char *name)
```

> Return function declaration called `name`, or `NULL` if not found.

