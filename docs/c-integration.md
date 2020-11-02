# Integrating with C

Having the ability to directly integrate with C programs allows you to give more power to your Skull programs.

This document assumes you have Skull installed. If you do not, refer to the [README](/README.md#setup).

Let us look at a simple example of how to call C functions from inside Skull:

```c
// hello.c

#include <stdio.h>

void hello(const char *name) {
	printf("hello, %s!\n", name);
}
```

and in Skull:

```python
# hello.sk

external hello(name: str)

hello("world")

return 0
```

Now, to compile these files together:

```
$ skull hello.sk -- hello.c -no-pie
$ ./hello
hello, world!
```

Everything after `--` is passed as arguments to `cc`, in this case, `hello.c` and `-no-pie`.

The `-no-pie` option is required we are compiling code with string literals.

Skull can also compile `.o` and `.so` files, but you will need to compile them yourself:

```
$ cc -c hello.c
$ skull hello.sk -- hello.o
$ ./hello
hello, world!
```

Skull does not check the types of your C function calls, or whether they even exist:

```
$ skull hello.sk -- -no-pie
./.hello.sk.o: In function `main':
main_module:(.text+0x2): undefined reference to `hello'
collect2: error: ld returned 1 exit status
```

Here, the linker is throwing an error as we forgot to include `hello.c` to our compilation.