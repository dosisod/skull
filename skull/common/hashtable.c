#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/common/hashtable.h"

HashTable *ht_create(void) {
	return Calloc(1, sizeof(HashTable));
}

unsigned ht_hash_key(const char *const key) {
	const size_t len = strlen(key);

	unsigned long long hash = 0;

	for RANGE(i, len) { // NOLINT
		hash += (unsigned)key[i] * i;
	}

	return hash % MAX_SLOTS;
}

bool ht_add(HashTable *const ht, const char *const key, void *const ptr) {
	if (!ptr || ht_get(ht, key)) {
		return false;
	}

	HashItem *head = &ht->slots[ht_hash_key(key)];

	if (!head->key) {
		head->key = key;
		head->data = ptr;
		return true;
	}

	HashItem *item = head;
	HashItem *last = head;

	while (item) {
		last = item;
		item = item->next;
	}

	HashItem *new_item;
	new_item = Calloc(1, sizeof *new_item);

	new_item->key = key;
	new_item->data = ptr;
	last->next = new_item;

	return true;
}

void *ht_get(const HashTable *const ht, const char *const key) {
	if (!ht) {
		return NULL;
	}

	const HashItem *item = &ht->slots[ht_hash_key(key)];

	while (item && item->key) {
		if (strcmp(item->key, key) == 0) {
			return item->data;
		}
		item = item->next;
	}

	return NULL;
}

void free_ht(HashTable *ht, void (*free_func)(void *)) {
	for RANGE(i, MAX_SLOTS) { // NOLINT
		HashItem *item = &ht->slots[i];

		if (free_func && item->data) {
			free_func(item->data);
		}
		item = item->next;

		while (item) {
			if (free_func) {
				free_func(item->data);
			}

			HashItem *tmp = item;
			item = item->next;
			free(tmp);
		}
	}

	free(ht);
}
