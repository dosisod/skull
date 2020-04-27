#include <stdbool.h>

#include "../src/parse/types.h"

#include "../test/testing.h"

bool test_make_new_type() {
	type_t *current=&TYPES_AVAILABLE;
	type_t *last=current;

	make_new_type("test_type");

	bool pass=false;
	while (current) {
		last=current;
		pass|=(strcmp(current->name, "test_type")==0);
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

	return !pass;
}

bool test_make_new_type_rejects_non_unique_type() {
	type_t *current=&TYPES_AVAILABLE;

	bool inserted1=make_new_type("test_type");
	bool inserted2=make_new_type("test_type");

	int count=0;

	while (current) {
		count+=(strcmp(current->name, "test_type")==0);
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

	return !(inserted1 && !inserted2 && count==1);
}

bool test_free_types() {
	make_new_type("test_type1");
	make_new_type("test_type2");
	make_new_type("test_type3");
	type_t *type1=TYPES_AVAILABLE.next;
	type_t *type2=TYPES_AVAILABLE.next->next;
	type_t *type3=TYPES_AVAILABLE.next->next->next;

	free_types();

	return !(
		type1->next==NULL &&
		type2->next==NULL &&
		type3->next==NULL &&
		TYPES_AVAILABLE.next==NULL
	);
}

bool test_append_default_types() {
	make_default_types();

	type_t *head=&TYPES_AVAILABLE;
	int count=0;
	while (head) {
		count++;
		head=head->next;
	}

	make_default_types();
	head=&TYPES_AVAILABLE;
	int new_count=0;
	while (head) {
		new_count++;
		head=head->next;
	}

	return !(new_count==count);
}

void types_test_self(bool *failed) {
	tests_t tests={
		test_make_new_type,
		test_make_new_type_rejects_non_unique_type,
		test_free_types,
		test_append_default_types,
		NULL
	};

	run_many_tests(__FILE__, tests, failed);
}
