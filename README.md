# skull

![](https://github.com/dosisod/skull/workflows/tests/badge.svg)

The skull programming language

## Syntax Proposal

```
import io.print

# this is a comment!

main[] int -> [
  print["hello world!"]

  return 0
]

main[]
```

## Building

```
$ git clone https://github.com/dosisod/skull
$ cd skull
$ mkdir build
$ make all
```

## Testing

Tests are best ran via a `pre-commit`:

```
# run tests, wont commit unless tests pass
$ git commit
```

To use these `pre-commit`s, run the following:

```
$ cp pre-commit .git/hooks/pre-commit
$ chmod +x .git/hooks/pre-commit
```

Tests may also be ran manually:

```
$ ./build/test/test
```

## Todo

Soon to come:

- [ ] Runnable skull files
- [ ] Skull to C function calls
- [ ] Rich skull std library
