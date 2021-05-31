# skull/common/io

```c
char *read_file(FILE *const fd)
```

> Return string read from file descriptor `fd`.

```c
FILE *open_file(const char *filename, bool is_RO)
```

> Open `filename`, handle errors if any.
> \
> Set `is_RO` to true if file should be opened as read-only.

