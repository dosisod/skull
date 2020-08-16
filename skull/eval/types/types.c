#include <stdlib.h>

#include "skull/common/str.h"
#include "skull/common/malloc.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/char.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/str.h"
#include "skull/eval/types/type.h"
#include "skull/eval/variable.h"

#include "skull/eval/types/types.h"

/*
Creates a new type called `name` with `bytes` bytes of memory.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char32_t *name, size_t bytes) {
	type_t *current=TYPES_AVAILABLE;
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

	last->next=new_type; // NOLINT

	return true;
}

/*
Returns pointer to type with name `name`.
*/
type_t *find_type(const char32_t *name) {
	type_t *head=TYPES_AVAILABLE;

	while (head) {
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
	//TYPE_BOOL is the last defined builtin type
	type_t *head=TYPE_BOOL.next;
	type_t *tmp;

	while (head) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}

	TYPE_BOOL.next=NULL;
}

struct type_t TYPE_BOOL = {
	.name=U"bool",
	.bytes=sizeof(bool),
	.to_string=&fmt_bool_type,
	.add=NULL,
	.subtract=NULL,
	.divide=NULL,
	.multiply=NULL,
	.next=NULL
};

struct type_t TYPE_INT = {
	.name=U"int",
	.bytes=sizeof(int64_t),
	.to_string=&fmt_int_type,
	.add=add_int_type,
	.subtract=sub_int_type,
	.divide=div_int_type,
	.multiply=mult_int_type,
	.next=&TYPE_BOOL
};

struct type_t TYPE_FLOAT = {
	.name=U"float",
	.bytes=sizeof(double),
	.to_string=&fmt_float_type,
	.add=add_float_type,
	.subtract=sub_float_type,
	.divide=div_float_type,
	.multiply=mult_float_type,
	.next=&TYPE_INT
};

struct type_t TYPE_CHAR = {
	.name=U"char",
	.bytes=sizeof(char32_t),
	.to_string=&fmt_char_type,
	.add=NULL,
	.subtract=NULL,
	.divide=NULL,
	.multiply=NULL,
	.next=&TYPE_FLOAT
};

struct type_t TYPE_STR = {
	.name=U"str",
	.bytes=sizeof(char32_t*),
	.to_string=&fmt_str_type,
	.add=add_str_type,
	.subtract=NULL,
	.divide=NULL,
	.multiply=NULL,
	.next=&TYPE_CHAR
};

struct type_t TYPE_TYPE = {
	.name=U"type",
	.bytes=sizeof(struct type_t*),
	.to_string=&fmt_type_type,
	.add=NULL,
	.subtract=NULL,
	.divide=NULL,
	.multiply=NULL,
	.next=&TYPE_STR
};

struct type_t *TYPES_AVAILABLE=&TYPE_TYPE;
