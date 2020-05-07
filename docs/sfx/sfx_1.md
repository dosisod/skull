# SFX 1 - Code Style

The below guidelines are not enforced, but are strongly recommended.

# Tabs or Spaces?

Skull programs should use 2 space indentation:

```
main[] i32 [
  print["hello world!"]

  return 0
]
```

No matter what you chose, your use of indentation should remain constant.

# Case style?

Function names should be `snake_case`:

```
function_name[] [
  i32 variable_name = 0

  return variable_name
]
```

Classes should be `PascalCase`:

```
TBD.
```

# File Size?

A single file should strive to contain 50 SLOC (Source lines of code).

This means comments, whitespace, or opening/closing brackets do not count.

# File Names?

File names should be succinctly describe what the code does.

If a file name cannot describe the code inside, the file is probably too big or complex.