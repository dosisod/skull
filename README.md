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

## Run via Docker

Versions and sizes (as of v0.9.0):

| Version | Size |
|---------|------|
| `alpine` | 312MB |
| `archlinux` | 1.62GB |
| `ubuntu` | 407MB |
| `alpine-dev` | 579MB |
| `archlinux-dev` | 2.35GB |
| `ubuntu-dev` | 1.32GB |

Running:

```
$ docker run --rm -it ghcr.io/dosisod/skull:<VERSION> bash
```

The reason these dockerfiles are so big is because they include development
packages such as GCC, which is needed for the final compilation of Skull programs.
This might change in the future though.

## Setup

Before you build, you will need to install some common development packages,
and LLVM 14.

#### Ubuntu

Note: the `llvm-14-dev` package is only available on Ubuntu Jammy (22.04).
If you want to use this in 21.04 or earlier, you will need to add the
[LLVM APT repositories](https://apt.llvm.org/) to your system (untested), or
build from [source](https://github.com/llvm/llvm-project#getting-the-source-code-and-building-llvm).

```
$ sudo apt install llvm-14-dev build-essential git xxd
```

You can also drop in `clang` instead of `gcc` (`gcc` is included in
`build-essential` package by default):

```
$ sudo apt install clang
```

#### Arch Linux

```
$ sudo pacman -S base-devel clang llvm llvm-libs git man-db vim
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

Install [https://docs.docker.com/engine/install/ubuntu/](docker).

In addition run:

```
$ sudo apt install clang-tidy-14
```

#### Arch Linux

Install [https://wiki.archlinux.org/title/Docker](docker).

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

## Syntax Highlighting

It is recommended to use the newer [tree-sitter syntax highlighter](https://github.com/dosisod/tree-sitter-skull)
instead of the older [Vim syntax highlighter](https://github.com/dosisod/vim-skull).

Once you do have it installed though you will automatically get syntax highlighting for
all `.sk` files!

## Todo

- [x] Runnable Skull files
- [x] C to Skull bindings
- [x] Skull to C bindings
- [ ] Rich Skull std library
