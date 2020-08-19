#include <stdbool.h>
#include <stdint.h>

#include "skull/common/str.h"
#include "skull/eval/types/types.h"

#include "test/testing.h"

TEST(make_new_type, {
	type_t *current=TYPES_AVAILABLE;

	make_new_type(U"test_type", 1);

	bool pass=false;
	while (current) {
		pass|=c32scmp(current->name, U"test_type");
		current=current->next;
	}

	current=(TYPES_AVAILABLE)->next;
	type_t *last=TYPES_AVAILABLE;

	if (!current) {
		return false;
	}

	while (current->next) {
		last=current;
		current=current->next;
	}

	free(current);
	last->next=NULL;

	return pass;
})

TEST(make_new_type_rejects_non_unique_type, {
	const bool inserted1=make_new_type(U"test_type", 1);

	type_t *type=find_type(U"test_type");
	if (!type) {
		return false;
	}

	const bool inserted2=make_new_type(U"test_type", 1);
	if (type->next) {
		return false;
	}

	free_types();

	return (
		inserted1 &&
		!inserted2
	);
})

TEST(find_type, {
	type_t *type=find_type(U"int");

	const bool pass=(type==&TYPE_INT);

	return pass;
})

TEST(free_types, {
	make_new_type(U"test_type", 1);

	free_types();
	return !TYPE_BOOL.next;
})

TEST(append_default_types, {
	type_t *head=TYPES_AVAILABLE;

	uint8_t count=0;
	while (head) {
		count++;
		head=head->next;
	}

	head=TYPES_AVAILABLE;

	uint8_t new_count=0;
	while (head) {
		new_count++;
		head=head->next;
	}

	free_types();

	return new_count==count;
})

void types_test_self(bool *pass) {
	tests_t tests={
		test_make_new_type,
		test_make_new_type_rejects_non_unique_type,
		test_find_type,
		test_free_types,
		test_append_default_types,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
