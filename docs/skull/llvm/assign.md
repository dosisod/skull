# skull/llvm/assign.c

```c
void llvm_make_var_def(AstNode **node)
```

> Builds a variable from `node`.

```c
void llvm_make_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

```c
void llvm_make_assign_(Variable *const var, const AstNode *const node)
```

> Internal function to build LLVM assignment from `node` to `var.

```c
void llvm_assign_identifier(Variable *const var, const AstNode *const node)
```

> Build LLVM to assign an existing identifier `node` to `var`.

