# skull/common/hashtable

```c
HashTable *ht_create(void)
```

> Create a new hashtable.

```c
unsigned ht_hash_key(const char *const key)
```

> Return hashed key for a given key.

```c
bool ht_add(HashTable *const ht, const char *const key, void *const ptr)
```

> Add key `key` with value `ptr` to hashtable `ht`.

```c
void *ht_get(const HashTable *const ht, const char *const key)
```

> Get value with key `key` from hashtable `ht`.
> \
> Return `NULL` if key does not exist.

```c
void free_ht(HashTable *ht, void (*free_func)(void *))
```

> Free a hashtable `ht`, and free each item with `free_func`.

