# Variables

Variables in Skull are pretty straight-foreward. They can be either constant
(cannot change), or mutable (can change). Immutable (constant) variables are
the default. Type aliases look similar to variable declarations, and can be
used to make an alias of an existing type.

## Constants

```
x := 1
```

Here we define the constant `x`, which has a value of `1`, and a type of `Int`.
`Int` is implied because the type of the expression `1` is `Int`, and since we
are using `:=` to declare out variable, Skull will auto-deduce the type so
that we don't have to. We can be explicit about what type we want to use if we
want:

```
x: Int = 0
```

Also note that we cannot re-assign `x`, since it is constant. If we do, we
will get an error:

```
x := 1

x = 2
```

```
$ skull file.sk
file.sk: Compilation error: line 3 column 1: cannot reassign const variable "x"
```

## Mutable Variables

If we want to be able to modify `x` later, we must declare it as mutable:

```
mut x := 1

x = 2
```

This program now compiles.

## Type Aliases

You can create an alias for an existing type:

```
Num := Int

age: Num = 100
```

Note that you will not be able to re-assign `Num`.
