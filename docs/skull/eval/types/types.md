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
const Type __attribute__((pure)) *find_type(const char *const name)
```

> Returns pointer to type with name `name`.

