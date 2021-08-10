# skull/codegen/entry

```c
int init_codegen_pipeline(const char *filename, char *file_contents)
```

> Run "parse" > "validate" > "codegen" pipeline on file contents `file_contents`.
> \
> Function takes ownership of `file_contents`.
> \
> Return `true` if errors occurred.

```c
static char *gen_filename(const char *filename, const char *ext)
```

> Create Skull filename based on `filename` with extention `ext`.

