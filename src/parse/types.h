#pragma once

typedef struct type_t {
	const char *name;

	struct type_t *next;
} type_t;

struct type_t TYPES_AVAILABLE = {
	.name="",
	.next=NULL
};

/*
Returns true if the type `name` was inserted.

Returns false if a type called `name` already exists, and was not inserted.
*/
bool make_new_type(const char *name) {
	type_t *current=&TYPES_AVAILABLE;
	type_t *last=current;

	while (current) {
		if (strcmp(current->name, name)==0) {
			return false;
		}
		last=current;
		current=current->next;
	}

	type_t *new_type=malloc(sizeof(type_t));
	new_type->name=name;
	new_type->next=NULL;

	last->next=new_type;

	return true;
}

/*
Free all defined types from `TYPES_AVAILABLE`, except for `TYPES_AVAILABLE`.
*/
void free_types() {
	type_t *head=(&TYPES_AVAILABLE)->next;
	type_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		free(tmp);
		tmp->next=NULL;
	}

	TYPES_AVAILABLE.next=NULL;
}

/*
Populate `TYPES_AVAILABLE` with all the builtin types.

Calling this function will reset all types defined in `TYPES_AVAILABLE`.
*/
void make_default_types() {
	free_types();

	make_new_type("i8");
	make_new_type("i16");
	make_new_type("i32");
	make_new_type("i64");
}
