# skull

![](https://github.com/dosisod/skull/workflows/tests/badge.svg)

The skull programming language

## Syntax Proposal

```
# this is a comment!

main[] i32 [
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
$ cd build
$ cmake ..
$ cmake --build .
```

## Testing

After building, run:

```
./test/Test
```

## Todo

Soon to come:

- [ ] Runnable skull files
- [ ] Skull to C function calls
- [ ] Rich skull std library
