#include <stdbool.h>

#include "../src/parse/types.h"

#include "../test/testing.h"

bool test_make_new_type() {
	type_t *current=&TYPES_AVAILABLE;
	type_t *last=current;

	make_new_type(L"test_type", 1);

	bool pass=false;
	while (current) {
		last=current;
		pass|=(wcscmp(current->name, L"test_type")==0);
		current=current->next;
	}

	current=&TYPES_AVAILABLE;
	last=current;

	while (current->next) {
		last=current;
		current=current->next;
	}

	free(current);
	last->next=NULL;

	return pass;
}

bool test_make_new_type_rejects_non_unique_type() {
	type_t *current=&TYPES_AVAILABLE;

	bool inserted1=make_new_type(L"test_type", 1);
	bool inserted2=make_new_type(L"test_type", 1);

	unsigned int count=0;

	while (current) {
		count+=(wcscmp(current->name, L"test_type")==0);
		current=current->next;
	}

	current=&TYPES_AVAILABLE;
	type_t *last=current;

	while (current->next) {
		last=current;
		current=current->next;
	}

	free(current);
	last->next=NULL;

	return (inserted1 && !inserted2 && count==1);
}

bool test_free_types() {
	make_new_type(L"test_type1", 1);
	make_new_type(L"test_type2", 1);
	make_new_type(L"test_type3", 1);
	type_t *type1=TYPES_AVAILABLE.next;
	type_t *type2=TYPES_AVAILABLE.next->next;
	type_t *type3=TYPES_AVAILABLE.next->next->next;

	free_types();

	return (
		type1->next==NULL &&
		type2->next==NULL &&
		type3->next==NULL &&
		TYPES_AVAILABLE.next==NULL
	);
}

bool test_append_default_types() {
	make_default_types();
	type_t *head=&TYPES_AVAILABLE;

	unsigned int count=0;
	while (head) {
		count++;
		head=head->next;
	}

	make_default_types();
	head=&TYPES_AVAILABLE;

	unsigned int new_count=0;
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
		test_free_types,
		test_append_default_types,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
