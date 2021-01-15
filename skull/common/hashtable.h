#pragma once

#define MAX_SLOTS 1024

typedef struct {
	const char *key;
	void *data;
	void *next;
} HashItem;

typedef struct {
	HashItem slots[MAX_SLOTS];
} HashTable;

HashTable *ht_create(void);

unsigned ht_hash_key(const char *);

_Bool ht_add(HashTable *, const char *, void *);

void *ht_get(HashTable *, const char *);

void free_ht(HashTable *, void (*)(void *));