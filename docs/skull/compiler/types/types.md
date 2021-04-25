# skull/compiler/types/types

```c
typedef const char * Type
```

> Types are simply named strings.

```c
typedef struct TemplateType {
	char *name;

	LLVMTypeRef *llvm_type;
}
```

> Similar to `Type` except that LLVM type is generated at runtime.

```c
Type __attribute__((pure)) find_type(const char *const name)
```

> Returns pointer to type with name `name`.

```c
bool add_alias(Type type, char *const alias)
```

> Add named `alias` for `type`.
> \
> Return `true` if alias was added, `false` if it already exists.

```c
char *mangle_types(Type *types, unsigned num_types, char ident)
```

> Return mangled type name which consists of `types`.
> \
> Use `ident` as an identifer for what the mandled type is.

