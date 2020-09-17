#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/eval/types/types.h"

#include "test/skull/eval/types/types.h"
#include "test/testing.h"

TEST(make_new_type, {
	Type *current = TYPES_AVAILABLE;

	make_new_type("test_type", 1);

	bool pass = false;
	while (current) {
		if (strcmp(current->name, "test_type") == 0) {
			pass = true;
		}
		current = current->next;
	}

	current = (TYPES_AVAILABLE)->next;
	Type *last = TYPES_AVAILABLE;

	ASSERT_TRUTHY(current);

	while (current->next) {
		last = current;
		current = current->next;
	}

	free(current);
	last->next = NULL;

	return pass;
})

TEST(make_new_type_rejects_non_unique_type, {
	const bool inserted1 = make_new_type("test_type", 1);

	Type *type = find_type("test_type");
	ASSERT_TRUTHY(type);

	const bool inserted2 = make_new_type("test_type", 1);
	ASSERT_FALSEY(type->next);

	free_types();

	ASSERT_TRUTHY(inserted1);
	ASSERT_FALSEY(inserted2);
	PASS;
})

TEST(find_type, {
	Type *type = find_type("int");

	ASSERT_EQUAL(type, &TYPE_INT);

	PASS;
})

TEST(free_types, {
	make_new_type("test_type", 1);

	free_types();
	ASSERT_FALSEY(TYPE_BOOL.next);

	PASS;
})

TEST(append_default_types, {
	Type *head = TYPES_AVAILABLE;

	unsigned count = 0;
	while (head) {
		count++;
		head = head->next;
	}

	head = TYPES_AVAILABLE;

	unsigned new_count = 0;
	while (head) {
		new_count++;
		head = head->next;
	}

	free_types();

	ASSERT_EQUAL(new_count, count);

	PASS;
})

TEST_SELF(types,
	test_make_new_type,
	test_make_new_type_rejects_non_unique_type,
	test_find_type,
	test_free_types,
	test_append_default_types
)
