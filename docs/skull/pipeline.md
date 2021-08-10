# skull/pipeline

```c
int run_pipeline(const char *filename, char *file_contents)
```

> Run "parse" > "validate" > "codegen" pipeline on `file_contents`.
> \
> Function takes ownership of `file_contents`.
> \
> Return `true` if errors occurred.

