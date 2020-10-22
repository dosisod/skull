# Integrating with C

Having the ability to directly integrate with C programs allows you to give more power to your Skull programs.

This document assumes you have Skull installed. If you do not, refer to the [README](/README.md#setup).

Let us look at a simple example of how to call C functions from inside Skull:

```c
// hello.c

#include <stdio.h>

void hello(void) {
	puts("hello from hello.c!");
}
```

and in Skull:

```python
# hello.sk

external hello

hello()

return 0
```

Now, to compile these files together:

```
$ skull hello.sk -- hello.c
$ ./hello
hello from hello.c!
```

Everything after `--` is passed as arguments to `cc`, in this case, `hello.c`.

Skull can also compile `.o` and `.so` files, but you will need to compile them yourself:

```
$ cc -c hello.c
$ skull hello.sk -- hello.o
$ ./hello
hello from hello.c!
```

For the time being, Skull can only run C functions prototypes like: `void f(void)`

In the future, Skull will be able to run C functions that can pass/return primative types (`int`, `float`, `rune`, etc).

Skull does not check the types of your function calls, or whether they exist:

```
$ skull hello.sk
./.hello.sk.o: In function `main':
main_module:(.text+0x2): undefined reference to `hello'
collect2: error: ld returned 1 exit status
```

Here, the linker is throwing an error, as we forgot to include `hello.c` to our compilation.

This integration process may change over time, namely to improve type safety, and cause less headaches.