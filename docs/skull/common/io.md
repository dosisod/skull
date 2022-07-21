# skull/common/io

```c
char *file_to_string(FILE *fd)
```

> Return string read from file descriptor `fd`.

```c
FILE *open_file(const char *filename, bool is_ro)
```

> Open `filename`, handle errors if any.
> \
> Set `is_ro` to true if file should be opened as read-only.

```c
char *gen_filename(const char *filename, const char *ext)
```

> Create Skull filename based on `filename` with extention `ext`.

