#include <stdbool.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/common/hashtable.h"

MAKE_VECTOR(HashItem)

/*
Create a new hashtable.
*/
HashTable *ht_create(void) {
	return Calloc(1, sizeof(HashTable));
}

/*
Return hashed key for a given key.
*/
__attribute__((pure)) unsigned ht_hash_key(const char *const key) {
	const size_t len = strlen(key);

	unsigned long long hash = 0;

	for RANGE(i, len) { // NOLINT
		hash += (unsigned)key[i] * i;
	}

	return hash % MAX_SLOTS;
}

/*
Add key `key` with value `ptr` to hashtable `ht`.
*/
bool ht_add(HashTable *const ht, const char *const key, void *const ptr) {
	if (!ptr || !ht || ht_get(ht, key)) return false;

	Vector **slot = &ht->slots[ht_hash_key(key)];
	Vector *items = *slot;

	if (!items) {
		items = make_vector();
		*slot = items;
	}

	HashItem *new_item;
	new_item = Calloc(1, sizeof *new_item);

	new_item->key = key;
	new_item->data = ptr;

	vector_push_HashItem(items, new_item);

	return true;
}

/*
Get value with key `key` from hashtable `ht`.

Return `NULL` if key does not exist.
*/
void *ht_get(const HashTable *const ht, const char *const key) {
	if (!ht) return NULL;

	const Vector *items = ht->slots[ht_hash_key(key)];
	if (!items) return NULL;

	for RANGE(i, items->length) {
		const HashItem *item = vector_at_HashItem(items, i);

		if (strcmp(item->key, key) == 0) return item->data;
	}

	return NULL;
}

/*
Free `item` using `free_func` (if passed).
*/
void free_ht_item(void (*free_func)(void *), void *item) {
	if (free_func) free_func(item);

	free(item);
}

/*
Free a hashtable `ht`, and free each item with `free_func`.
*/
void free_ht(HashTable *ht, void (*free_func)(void *)) {
	if (!ht) return;

	for RANGE(i, MAX_SLOTS) { // NOLINT
		Vector *items = ht->slots[i];

		if (items) free_vector2(items, free_ht_item, free_func);
	}

	free(ht);
}
