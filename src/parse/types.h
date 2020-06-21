#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/common/str.h"
#include "../../src/common/malloc.h"

typedef struct type_t {
	const char32_t *name;
	size_t bytes;

	struct type_t *next;
} type_t;

struct type_t TYPES_AVAILABLE = {
	.name=U"",
	.bytes=0,
	.next=NULL
};

/*
Creates a new type named `type` that allocates `bytes` bytes.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char32_t *name, size_t bytes) {
	type_t *current=&TYPES_AVAILABLE;
	type_t *last=current;

	while (current) {
		if (c32scmp(current->name, name)) {
			return false;
		}
		last=current;
		current=current->next;
	}

	type_t *new_type=malloc(sizeof(type_t));
	DIE_IF_MALLOC_FAILS(new_type);

	new_type->name=name;
	new_type->bytes=bytes;
	new_type->next=NULL;

	last->next=new_type;

	return true;
}

/*
Returns pointer to type with name `name`.
*/
type_t *find_type(const char32_t *name) {
	type_t *head=&TYPES_AVAILABLE;

	while (head!=NULL) {
		if (c32scmp(name, head->name)) {
			return head;
		}
		head=head->next;
	}

	return NULL;
}

/*
Free all defined types from `TYPES_AVAILABLE`, except for `TYPES_AVAILABLE`.
*/
void free_types(void) {
	type_t *head=(&TYPES_AVAILABLE)->next;
	type_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}

	TYPES_AVAILABLE.next=NULL;
}

/*
Populate `TYPES_AVAILABLE` with all the builtin types.

Calling this function will reset all types defined in `TYPES_AVAILABLE`.
*/
void make_default_types(void) {
	free_types();

	make_new_type(U"bool", 1);
	make_new_type(U"int", 8);
	make_new_type(U"float", 16);
	make_new_type(U"char", sizeof(char32_t));
	make_new_type(U"str", sizeof(char32_t*));
}
