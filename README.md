# skull

![](https://github.com/dosisod/skull/workflows/tests/badge.svg)

The Skull Programming Language

## Syntax Proposal

```python
import io.print

# this is a comment!

func() {
  print("hello world!")
}

func()

return 0
```

## Setup

Before you build, you will need to install LLVM dev dependencies:

```
$ sudo apt install llvm-10-dev
```

## Building and Running

```
$ git clone https://github.com/dosisod/skull
$ cd skull
$ sudo make install
$ skull
```

## Testing

Install dependencies:

```
$ sudo apt install clang-tidy-10
```

Tests are best ran via a `pre-commit`:

```
# run tests, wont commit unless tests pass
$ git commit
```

To use the `pre-commit`, run the following:

```
$ ln -sf $(pwd)/pre-commit .git/hooks/pre-commit
```

Tests may also be ran manually:

```
$ make test && ./build/test/test
```

## Todo

- [x] Runnable Skull files
- [x] C to Skull bindings
- [x] Skull to C bindings
- [ ] Rich Skull std library
