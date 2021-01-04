# skull/eval/types/types

```c
typedef struct Type {
	const char *name;

	LLVMTypeRef (*llvm_type)(void);

	char **aliases;
	unsigned num_aliases;
}
```

> `Type` is an abstraction on top of the existing LLVM type system.
> \
> `name` is the name of the type.
> \
> `llvm_type` is a pointer to a function that returns the desired LLVM type.
> \
> `aliases` are alternative names for the same type `name`.
> \
> `num_aliases` is the number of aliases for a given type.

```c
const Type __attribute__((pure)) *find_type(const char *const name)
```

> Returns pointer to type with name `name`.

```c
bool add_alias(Type *const type, char *const alias)
```

> Add named `alias` for `type`.
> \
> Return `true` if alias was added, `false` if it already exists.

