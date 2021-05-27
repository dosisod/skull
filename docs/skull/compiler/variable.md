# skull/compiler/variable

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

	LLVMValueRef ref;
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
> `ref` stores the actual LLVM alloca/constant/global.
> \
> `was_reassigned` stores whether a (mutable) variable was reassigned.
> \
> `was_read` stores whether a variable was read from or not.
> \
> `implicitly_typed` is true when variable type is auto-deduced.

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
void variable_no_warnings(Variable *var)
```

> Disable warnings on variable `var`.

