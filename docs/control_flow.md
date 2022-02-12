# Control Flow

Constructs like if statements, while loops, and for loops can all be used to
change how the flow of a program works.

## If blocks

An if block has 2 parts: the condition, and the body. When the condition is
true, the code inside the brackets is executed. For example:

```
if true {
  # do something
}
```

## Elif blocks

`elif` (AKA else if) blocks are used to run a second condition, if the first
`if` condition fails. For example:

```
if false {
  # wont run
}
elif true {
  # will run
}
```

## Else blocks

`else` blocks run when non of the above `if`/`elif` conditions are met. For
example:

```
if false {
  # wont run
}
else {
  # will run
}
```

## While blocks

`while` blocks run a block of code as long as the condition is true. For
example:

```
while true {
  # do something forever
}
```

This will cause an infinite loop, since the condition is always true, and
there is no way to get out of it. If we want to exit out of a loop early,
we can use `break`:

```
while true {
  # do something once
  break
}
```

Here we break at the end of the block. After breaking, the program continues
to execute the code just after the closing (`}`) bracket.

If we want to jump back to the start of a loop, we can use `continue`:

```
while true {
  if some_condition {
    continue
  }

  # do something
}
```

Here, we will go to the top of the code block that is inside the while loop,
but only if `some_condition` is true.

## Return

When we want to return a value from a function, we can use the `return`
statement:

```
f() Int { return 1 }

x := f()

# x is now equal to 1
```

## Unreachable

Sometimes the compiler is not smart enough to tell when a certain condition
is impossible to reach, or perhaps you are calling a library function which
never returns. To indicate that no code should go under a certain line, you
can use `unreachable`:

```
f()
unreachable

# error: unreachable code
x := 1
```
