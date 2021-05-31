# skull/setup_main

```c
int setup_main(int argc, char *argv[])
```

> Actual `main` function, can be called by external programs.

```c
static int build_file(char *filename)
```

> Build a .ll file from a .sk file `filename`.

```c
static char *gen_filename(const char *filename)
```

> Create Skull filename based on `filename`.

