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
> `elements` stores the actual elements of the vector.

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
void *vector_at(const Vector *const v, size_t index)
```

> Return the element at index `index` of the vector `v`, or NULL if out-of-bound.

```c
void free_vector(Vector *v, void (*free_func)(void *))
```

> Free vector `v`, and any memory it might have allocated via `free_func`.

```c
void free_vector2(Vector *v, void (*free_func)(void (*)(void *), void *), void (*free_func2)(void *))
```

> Free vector `v` using `free_func` to handle nested/managed objects
> (ie, `HashTable`), and `free_func2` to free the data in those objects.

