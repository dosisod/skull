# skullc

Experimental LLVM front end for Skull.

## Building / Running

To build, you will need some basic packages:

```
$ sudo apt install llvm-9 llvm-9-dev
```

Building:

```
$ ./skullc
```

This will output a new binary called `main`:

```
$ ./main
$ echo $?
1
```