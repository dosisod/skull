# Getting Started

First, make sure to [install Skull](/README.md#setup).

Verify that it is installed by running `skull --version`.

To create a basic hello world program, put the following into a file ending in `.sk`:

```
external puts(s: Str)

puts("hello world!")

return 0
```

Here we have to declare an external function, `puts`, since there is no built-in
print function (yet). `puts` is a part of the C library, and will get linked when
we compile. The `return` is not needed, and defaults to `0` if it is not specified.

You can compile and run like so:

```
$ skull filename.sk -- -no-pie
$ ./filename
hello world!
```

That's it! The `-no-pie` flag is needed because we are trying to link with `puts`.
This shouldn't be needed in the future, but is required for now.
