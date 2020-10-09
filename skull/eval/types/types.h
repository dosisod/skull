#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Variable Variable;

typedef struct Type {
	const char *name;
	size_t bytes;

	char *(*to_string)(const Variable *);

	struct Type *next;
} Type;

Type TYPE_BOOL;
Type TYPE_INT;
Type TYPE_FLOAT;
Type TYPE_RUNE;
Type TYPE_STR;

Type *TYPES_AVAILABLE;

bool make_new_type(const char *const, size_t);

Type *find_type(const char *const);

void free_types(void);
