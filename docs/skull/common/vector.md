# skull/common/vector

```c
typedef struct {
	size_t length;
	size_t max;
	void **elements;
}
```

> Simple Vector implementation.
> \
> `length` stores the length of the vector.
> \
> `max` stores the max length needed before reallocation.
> \
> `elements` stores the actual elements.

```c
void vector_push(Vector *v, void *ptr)
```

> Push pointer `ptr` to vector `v`. Scale vector if needed.

```c
void *vector_pop(Vector *v)
```

> Pop and return last element from vector `v`.

```c
Vector *make_vector(void)
```

> Return new vector.

```c
void free_vector(Vector *v, void (*free_func)(void *))
```

> Free vector `v`.

