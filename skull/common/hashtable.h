#pragma once

#include "skull/common/vector.h"

#define MAX_SLOTS 64

typedef struct HashItem {
	const char *key;
	void *data;
} HashItem;

typedef struct {
	Vector *slots[MAX_SLOTS];
} HashTable;

HashTable *make_ht(void);
_Bool ht_add(HashTable *const, const char *const, void *const);
void *ht_get(const HashTable *const, const char *const);
void free_ht(HashTable *, void (*)(void *));
