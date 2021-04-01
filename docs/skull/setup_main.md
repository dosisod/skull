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
void generate_llvm(const char *filename, char *file_contents)
```

> Create a module named `filename` and a main function called
> `main_func_name` from `file_contents`.

```c
char *create_llvm_main_func(const char *filename)
```

> Convert/mangle `filename` into suitable name for "main" method for module.

```c
char *gen_filename(const char *filename)
```

> Create Skull filename based on `filename`.

