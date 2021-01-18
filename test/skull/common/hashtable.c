#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/hashtable.h"

#include "test/skull/common/hashtable.h"
#include "test/testing.h"

TEST(hashtable_create_and_free, {
	HashTable *ht = ht_create();

	ASSERT_TRUTHY(ht);

	free_ht(ht, NULL);
})

TEST(hashtable_add_and_get, {
	HashTable *ht = ht_create();

	int value = 123;
	ht_add(ht, "key", &value);
	int *found = ht_get(ht, "key");

	ASSERT_TRUTHY(found);
	ASSERT_EQUAL(value, *found);

	free_ht(ht, NULL);
})

TEST(hashtable_add_to_existing_slot, {
	HashTable *ht = ht_create();

	// simulate adding entry into slot that already has an entry
	unsigned index = ht_hash_key("entry2");
	ht->slots[index].key = "entry1";

	int value = 123;
	ht_add(ht, "entry2", &value);
	int *found = ht_get(ht, "entry2");

	ASSERT_EQUAL(strcmp(ht->slots[index].key, "entry1"), 0);
	ASSERT_TRUTHY(found);
	ASSERT_EQUAL(value, *found);

	free_ht(ht, NULL);
})

TEST(hashtable_cannot_add_null, {
	HashTable *ht = ht_create();

	ASSERT_FALSEY(ht_add(ht, "key", NULL));
	ASSERT_FALSEY(ht_get(ht, "key"));

	free_ht(ht, NULL);
})

TEST(hashtable_cannot_add_key_twice, {
	HashTable *ht = ht_create();

	int value = 123;
	ht_add(ht, "key", &value);

	ASSERT_FALSEY(ht_add(ht, "key", &value));

	free_ht(ht, NULL);
})

TEST(hashtable_with_null, {
	ht_get(NULL, "");
})

TEST_SELF(hashtable,
	test_hashtable_create_and_free,
	test_hashtable_add_and_get,
	test_hashtable_add_to_existing_slot,
	test_hashtable_cannot_add_null,
	test_hashtable_cannot_add_key_twice,
	test_hashtable_with_null
)
