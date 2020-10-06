#pragma once

#include <stdbool.h>
#include <uchar.h>

typedef struct Variable Variable;

typedef Variable *(*OperFunc)(const Variable *, const Variable *);

typedef struct Type {
	const char *name;
	size_t bytes;

	OperFunc add;
	OperFunc subtract;
	OperFunc divide;
	OperFunc multiply;

	char32_t *(*to_string)(const Variable *);

	struct Type *next;
} Type;

Type TYPE_BOOL;
Type TYPE_INT;
Type TYPE_FLOAT;
Type TYPE_RUNE;
Type TYPE_STR;
Type TYPE_TYPE;

Type *TYPES_AVAILABLE;

bool make_new_type(const char *, size_t);

Type *find_type(const char *);

void free_types(void);
