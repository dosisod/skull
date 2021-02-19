# skull/common/local

```c
void setup_locale(void)
```

> If system local is not set, default to "C.UTF-8". Throw assertion if system
> does not support UTF-32, and exit if unable to setup a UTF-8 environment.

