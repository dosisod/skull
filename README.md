# skull

![](https://github.com/dosisod/skull/workflows/tests/badge.svg)

The Skull Programming Language

## Syntax Proposal

```python
import io.print

# this is a comment!

main() Int {
  print("hello world!")

  return 0
}

return main()
```

## Setup

Before you build, you will need to install LLVM dev dependencies. Currently
Skull uses LLVM 13, but should work on version 12 as well.

Assumes you have basic `make`, `gcc`, `git`, and other common compiler tools.

#### Ubuntu

```
$ sudo apt install llvm-13-dev # impish (21.10)
$ sudo apt install llvm-12-dev # focal (20.04)
```

#### Arch Linux

```
$ sudo pacman -S clang llvm llvm-libs
```

## Building and Running

```
$ git clone https://github.com/dosisod/skull
$ cd skull
$ sudo make install
$ skull
```

## Testing

Note: docker is required for running [https://github.com/koalaman/shellcheck](shellcheck)
in the pre-commit file.

Install additional dependencies:

#### Ubuntu

```
$ sudo apt install clang-tidy-13 # impish
$ sudo apt install clang-tidy-12 # focal
```

### Running Tests

Tests are best ran via a `pre-commit`:

```
# run tests, wont commit unless tests pass
$ git commit
```

To use the `pre-commit`, run the following:

```
$ ln -sf $(pwd)/pre-commit .git/hooks/pre-commit
```

The first time you install Skull globally, you will need to also install
the header files, which should only need to be done once (until they are
updated). You can do this by running:

```
$ sudo make install-dev
```

Tests may also be ran manually (no pre-commit):

```
$ make test e2e
$ ./build/test/test
$ ./test/sh/main.sh
```

## Todo

- [x] Runnable Skull files
- [x] C to Skull bindings
- [x] Skull to C bindings
- [ ] Rich Skull std library
