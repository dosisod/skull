# skull/setup_main

```c
int setup_main(int argc, char *argv[])
```

> Actual `main` function, can be called by external programs.

```c
int build_file(char *filename)
```

> Build a .ll file from a .sk file `filename`.

```c
LLVMModuleRef generate_llvm(const char *module_name, const char *main_func_name, char *file_contents)
```

> Create a module named `module_name` and a main function called
> `main_func_name` from `file_contents`.

```c
char *create_llvm_main_func(const char *filename)
```

> Convert/mangle `filename` into suitable name for "main" method for module.

```c
char *create_llvm_filename(const char *filename)
```

> Modify filename to add `.ll` extention to it.

