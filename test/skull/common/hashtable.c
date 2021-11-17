#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/hashtable.c"

#include "test/skull/common/hashtable.h"
#include "test/testing.h"

bool test_hashtable_create_and_free(void) {
	HashTable *ht = make_ht();

	ASSERT_TRUTHY(ht);

	free_ht(ht, NULL);

	PASS
}

bool test_hashtable_add_and_get(void) {
	HashTable *ht = make_ht();

	int value = 123;
	ht_add(ht, "key", &value);
	int *found = ht_get(ht, "key");

	ASSERT_TRUTHY(found);
	ASSERT_EQUAL(value, *found);

	free_ht(ht, NULL);

	PASS
}

bool test_hashtable_add_to_existing_slot(void) {
	HashTable *ht = make_ht();

	// simulate adding entry into slot that already has an entry
	int value = 123;
	ht_add(ht, "entry2", &value);
	unsigned index = ht_hash_key("entry2");
	HashItem *item = vector_at(ht->slots[index], 0);
	item->key = "entry1";

	ht_add(ht, "entry2", &value);
	int *found = ht_get(ht, "entry2");

	ASSERT_EQUAL(strcmp(item->key, "entry1"), 0);
	ASSERT_TRUTHY(found);
	ASSERT_EQUAL(value, *found);

	free_ht(ht, NULL);

	PASS
}

bool test_hashtable_cannot_add_null(void) {
	HashTable *ht = make_ht();

	ASSERT_FALSEY(ht_add(ht, "key", NULL));
	ASSERT_FALSEY(ht_get(ht, "key"));

	free_ht(ht, NULL);

	PASS
}

bool test_hashtable_cannot_add_key_twice(void) {
	HashTable *ht = make_ht();

	int value = 123;
	ht_add(ht, "key", &value);

	ASSERT_FALSEY(ht_add(ht, "key", &value));

	free_ht(ht, NULL);

	PASS
}

bool __attribute__((const)) test_hashtable_with_nulls(void) {
	ASSERT_FALSEY(ht_get(NULL, "key"));
	ASSERT_FALSEY(ht_add(NULL, "key", NULL));
	free_ht(NULL, NULL);

	PASS
}

void hashtable_test_self(bool *pass) {
	RUN_ALL(
		test_hashtable_create_and_free,
		test_hashtable_add_and_get,
		test_hashtable_add_to_existing_slot,
		test_hashtable_cannot_add_null,
		test_hashtable_cannot_add_key_twice,
		test_hashtable_with_nulls
	)
}
