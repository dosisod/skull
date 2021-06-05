# skull/codegen/entry

```c
int init_codegen_pipeline(const char *filename, char *file_contents)
```

> Setup codegen pipeline, including exporting of file.
> \
> Function takes ownership of `file_contents`.

```c
static char *gen_filename(const char *filename, const char *ext)
```

> Create Skull filename based on `filename` with extention `ext`.

```c
static bool codegen_pipeline(char *const str_)
```

> Run "parse" > "validate" > "codegen" pipeline on file contents `str_`.
> \
> Return `true` if errors occurred.

