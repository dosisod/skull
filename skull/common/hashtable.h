#pragma once

#include "skull/common/vector.h"

#define MAX_SLOTS 1024

typedef struct {
	const char *key;
	void *data;
} HashItem;

typedef struct {
	Vector *slots[MAX_SLOTS];
} HashTable;

HashTable *ht_create(void);

unsigned ht_hash_key(const char *const);

_Bool ht_add(HashTable *const, const char *const, void *const);

void *ht_get(const HashTable *const, const char *const);

void free_ht(HashTable *, void (*)(void *));
