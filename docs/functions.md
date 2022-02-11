# Functions

Functions are pieces of code that get executed when a function expression is
called. Functions can have parameters, which is basically a list of
expressions. Functions can also return values.

The basic anatomy of a function is as follows:

```
function_name(param1: type1, param2: type2) return_type {
  return something
}
```

Here, we are defining a function called `function_name`, which has 2
parameters, `param1` and `param2`, which are of type `type1` and `type2`
respectively. It returns a value of type `return_type`.

Functions that don't return a type are defined in the same way, except they
don't have a type after the closing paranthesis:

```
f() {
  # body
}
```

## Exporting Functions

If we want to expose certain functions to other parts of our codebase, we have
to `export` the function:

```
export f() {
  # do something
}
```

Note that all exported functions must be defined at the top level (not in
another function or if block). For example, the following is not allowed:

```
if true {
  # error: cannot declare nested function "f" as external or exported
  export f() {
    # not allowed
  }
}
```

## External Functions

Sometimes we want to use a function that is used in an external library, or
was written in a different programming language. We have to declare the type
of the function so that we have proper types when calling it (this is called
"forwarding" a function). To do this, we define a function without a body,
and prepend the `external` keyword to it:

```
external f()
```

This declares a function called `f`, which we can now use in our function.

> Note: If you use an external function, you will get an error if Skull
> cannot find an implementation for the function you forward declared.
> You might have to create an object file (using the `-c` flag), and then
> link with an existing build system (if needed).

## Nested Functions

Functions can be nested inside each-other:

```
outer() {
  inner() {
    # do something
  }

  inner()
}
```

Note that you will not be able to call `inner` outside of the `outer` function.
Also, since Skull does not properly handle closures (yet), you will not be able
to use any mutable variables in `inner` that have been defined in `outer`. The
following example does not work (yet):

```
outer() {
  mut x := 1

  inner() {
    y := x
  }

  inner()
}
```
