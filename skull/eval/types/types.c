#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/malloc.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"
#include "skull/eval/variable.h"

#include "skull/eval/types/types.h"

/*
Creates a new type called `name` with `bytes` bytes of memory.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char *const name, size_t bytes) {
	Type *current = TYPES_AVAILABLE;
	Type *last = current;

	while (current) {
		if (strcmp(current->name, name) == 0) {
			return false;
		}
		last = current;
		current = current->next;
	}

	Type *new_type;
	new_type = malloc(sizeof *new_type);
	DIE_IF_MALLOC_FAILS(new_type);

	new_type->name = name;
	new_type->bytes = bytes;
	new_type->next = NULL;

	last->next = new_type; // NOLINT

	return true;
}

/*
Returns pointer to type with name `name`.
*/
Type __attribute__((pure)) *find_type(const char *const name) {
	Type *head = TYPES_AVAILABLE;

	while (head) {
		if (strcmp(name, head->name) == 0) {
			return head;
		}
		head = head->next;
	}

	return NULL;
}

/*
Free all non-builtin types.
*/
void free_types(void) {
	//TYPE_BOOL is the last defined builtin type
	Type *head = TYPE_BOOL.next;
	Type *tmp;

	while (head) {
		tmp = head;
		head = head->next;
		tmp->next = NULL;
		free(tmp);
	}

	TYPE_BOOL.next = NULL;
}

Type TYPE_BOOL = {
	.name = "bool",
	.bytes = sizeof(bool),
	.to_string = &fmt_bool_type,
	.add = NULL,
	.subtract = NULL,
	.divide = NULL,
	.multiply = NULL,
	.next = NULL
};

Type TYPE_INT = {
	.name = "int",
	.bytes = sizeof(SkullInt),
	.to_string = &fmt_int_type,
	.add = add_int_type,
	.subtract = sub_int_type,
	.divide = div_int_type,
	.multiply = mult_int_type,
	.next = &TYPE_BOOL
};

Type TYPE_FLOAT = {
	.name = "float",
	.bytes = sizeof(SkullFloat),
	.to_string = &fmt_float_type,
	.add = add_float_type,
	.subtract = sub_float_type,
	.divide = div_float_type,
	.multiply = mult_float_type,
	.next = &TYPE_INT
};

Type TYPE_RUNE = {
	.name = "rune",
	.bytes = sizeof(SkullRune),
	.to_string = &fmt_rune_type,
	.add = NULL,
	.subtract = NULL,
	.divide = NULL,
	.multiply = NULL,
	.next = &TYPE_FLOAT
};

Type TYPE_STR = {
	.name = "str",
	.bytes = sizeof(SkullStr),
	.to_string = &fmt_str_type,
	.add = add_str_type,
	.subtract = NULL,
	.divide = NULL,
	.multiply = NULL,
	.next = &TYPE_RUNE
};

Type *TYPES_AVAILABLE = &TYPE_STR;
