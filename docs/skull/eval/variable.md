# skull/eval/variable

```c
typedef struct Variable {
	const Type *type;
	char *name;
	_Bool is_const;
	_Bool is_const_lit;
	_Bool is_global;

	LLVMValueRef llvm_value;
}
```

> `Variable` acts as a more strict abstraction on top of LLVM.
> \
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
> `llvm_value` stores the actual LLVM alloca/constant/global.

```c
Variable *make_variable(const Type *const type, const char32_t *const name, bool is_const)
```

> Make a variable called `name` with type `type`, and make it const if
> `is_const` is true.
> \
> Returns `NULL` if var cannot be created, else pointer to created var.

```c
void free_variable(Variable *var)
```

> Free variable `var`.

