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
$ sudo apt install llvm-9-dev
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
$ sudo apt install clang-tidy-9
```

Tests are best ran via a `pre-commit`:

```
# run tests, wont commit unless tests pass
$ git commit
```

To use the `pre-commit`, run the following:

```
$ cp pre-commit .git/hooks/pre-commit
$ chmod +x .git/hooks/pre-commit
```

Tests may also be ran manually:

```
$ make test && ./build/test/test
```

## Todo

- [x] Runnable Skull files
- [ ] Skull to/from C bindings
- [ ] Rich Skull std library
