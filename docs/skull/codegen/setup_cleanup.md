# skull/codegen/setup_cleanup

```c
bool write_file(char *filename)
```

> Write LLVM code to `filename`, return whether error occured.
> \
> Function takes control of `filename`.

```c
bool generate_llvm(const char *filename, char *file_contents)
```

> Create a module named `filename` from `file_contents`.
> \
> Return `true` if errors occurred.

