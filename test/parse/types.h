#include <stdbool.h>
#include <stdint.h>

#include "../../src/parse/types.h"
#include "../../test/testing.h"

bool test_make_new_type() {
	type_t *current=&TYPES_AVAILABLE;

	make_new_type(L"test_type", 1);

	bool pass=false;
	while (current) {
		pass|=(wcscmp(current->name, L"test_type")==0);
		current=current->next;
	}

	current=(&TYPES_AVAILABLE)->next;
	type_t *last=&TYPES_AVAILABLE;

	if (current==NULL) {
		return false;
	}

	while (current->next!=NULL) {
		last=current;
		current=current->next;
	}

	free(current);
	last->next=NULL;

	return pass;
}

bool test_make_new_type_rejects_non_unique_type() {
	type_t *current=&TYPES_AVAILABLE;

	const bool inserted1=make_new_type(L"test_type", 1);
	const bool inserted2=make_new_type(L"test_type", 1);

	uint8_t count=0;

	while (current) {
		count+=(wcscmp(current->name, L"test_type")==0);
		current=current->next;
	}

	current=(&TYPES_AVAILABLE)->next;
	type_t *last=&TYPES_AVAILABLE;

	while (current) {
		last=current;
		current=current->next;
	}

	free(current);
	last->next=NULL;

	return (
		inserted1 &&
		!inserted2 &&
		count==1
	);
}

bool test_find_type() {
	make_default_types();

	type_t *type=find_type(L"int");

	const bool pass=(type!=NULL);

	free_types();
	return pass;
}

bool test_free_types() {
	make_new_type(L"test_type", 1);

	free_types();

	return TYPES_AVAILABLE.next==NULL;
}

bool test_append_default_types() {
	make_default_types();
	type_t *head=&TYPES_AVAILABLE;

	uint8_t count=0;
	while (head) {
		count++;
		head=head->next;
	}

	make_default_types();
	head=&TYPES_AVAILABLE;

	uint8_t new_count=0;
	while (head) {
		new_count++;
		head=head->next;
	}

	free_types();

	return new_count==count;
}

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
