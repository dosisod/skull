# skull/setup_main

```c
int setup_main(int argc, char *argv[])
```

> Actual `main` function, can be called by external programs.

```c
LLVMModuleRef generate_llvm(const char *module_name, const char *main_func_name, char *file_contents)
```

> Create a module named `module_name` and a main function called
> `main_func_name` from `file_contents`.

```c
char *create_llvm_filename(const char *filename)
```

> Modify filename to add `.ll` extention to it.

