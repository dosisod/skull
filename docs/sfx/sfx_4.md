# SFX 4 - The Pipe Operator

Many C style languages use `|` to do binary OR operations, but in Skull,
`or` is used instead. This opens the door for more possibilities in terms
of operators. For example, using `|` for shell-style piping operations:

```python
f(x: Int) Int {
  return x
}

num := 1 | f
# num is 1


add(a: Int, b: Int) Int {
  return a + b
}

added := 1 | add(2)
# added is 3


echo(str: Str) {
  puts(str)
}

"hello world" | echo
```

### Why?

For sake of readablity. Take the following 2 examples for instance:

```python
add(a: Int, b: Int) Int {
  return a + b
}

add_1(x: Int) Int {
  return x + 1
}

square(x: Int) Int {
  return x * x
}

v1 := add(square(add_1(2)), 3)

# compared to

v2 := 2 | add_1 | square | add(3)
```

Here, `v2` can easily be read left-to-right, whereas `v1` needs to be read
inside-out (and then left-to-right for the `3`).

### Alternative Syntaxes

| Language | Syntax |
|:-------- |:------ |
| Javascript`*` | `|>` |
| Elixr | `|>` |
| C#`*` | `|>` |
| R | `%>%` |
| Shell | `|` |
| Haskell | `&` |

`*` Experimental/in draft.
