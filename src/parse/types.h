#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/common/str.h"
#include "../../src/common/malloc.h"

typedef struct type_t {
	const char32_t *name;
	size_t bytes;

	struct type_t *next;
} type_t;

struct type_t TYPE_BOOL = {
	.name=U"bool",
	.bytes=sizeof(bool),
	.next=NULL
};

struct type_t TYPE_INT = {
	.name=U"int",
	.bytes=sizeof(int64_t),
	.next=&TYPE_BOOL
};

struct type_t TYPE_FLOAT = {
	.name=U"float",
	.bytes=sizeof(long double),
	.next=&TYPE_INT
};

struct type_t TYPE_CHAR = {
	.name=U"char",
	.bytes=sizeof(char32_t),
	.next=&TYPE_FLOAT
};

struct type_t TYPE_STR = {
	.name=U"str",
	.bytes=sizeof(char32_t*),
	.next=&TYPE_CHAR
};

struct type_t *TYPES_AVAILABLE=&TYPE_STR;

/*
Creates a new type named `type` that allocates `bytes` bytes.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char32_t *name, size_t bytes) {
	type_t *current=TYPES_AVAILABLE;
	type_t *last=current;

	while (current!=NULL) {
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

	last->next=new_type; // NOLINT

	return true;
}

/*
Returns pointer to type with name `name`.
*/
type_t *find_type(const char32_t *name) {
	type_t *head=TYPES_AVAILABLE;

	while (head!=NULL) {
		if (c32scmp(name, head->name)) {
			return head;
		}
		head=head->next;
	}

	return NULL;
}

/*
Free all non-builtin types.
*/
void free_types(void) {
	//TYPE_BOOL is the last defined builtin
	type_t *head=TYPE_BOOL.next;
	type_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}

	TYPE_BOOL.next=NULL;
}
