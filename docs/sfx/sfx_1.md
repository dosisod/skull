# SFX 1 - Code Style

The below guidelines are not enforced, but are strongly recommended.

# Tabs or Spaces?

Skull programs should use 2 space indentation:

```python
main[] int -> [
  print["hello world!"]

  return 0
]
```

No matter what you chose, your use of indentation should remain constant.

# File Encoding?

Files must be stored in UTF-8 format.

# Case Style?

Function and variable names should be in `snake_case`:

```python
function_name[] int -> [
  variable_name: int = 0

  return variable_name
]
```

Classes should be in `PascalCase`:

```
TBD.
```

# File Size?

A single file should strive to contain 50 SLOC (Source lines of code) or less.

Comments, whitespace, and opening/closing brackets do not count as source code.

# File Names?

File names should succinctly describe what the code inside does.

If a file name cannot describe the code inside, the file is probably too big or complex.