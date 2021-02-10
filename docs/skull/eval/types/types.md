# skull/eval/types/types

```c
typedef struct Type {
	const char *name;

	LLVMTypeRef (*llvm_type)(void);
}
```

> `Type` is an abstraction on top of the existing LLVM type system.
> \
> `name` is the name of the type.
> \
> `llvm_type` is a pointer to a function that returns the desired LLVM type.

```c
typedef struct TemplateType {
	char *name;

	LLVMTypeRef *llvm_type;
}
```

> Similar to `Type` except that `llvm_type` is stored directly, and is not
> a function.

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

```c
char *mangle_types(const Type **types, unsigned num_types, char ident)
```

> Return mangled type name which consists of `types`.
> \
> Use `ident` as an identifer for what the mandled type is.

```c
TemplateType *add_template_type(char *name, LLVMTypeRef *llvm_type)
```

> Return newly created template type with called `name` of type `llvm_type`.

