#pragma once

#include <stdbool.h>
#include <uchar.h>

struct Variable;

typedef struct Variable *(*OperFunc)(const struct Variable *, const struct Variable *);

typedef struct Type {
	const char *name;
	size_t bytes;

	OperFunc add;
	OperFunc subtract;
	OperFunc divide;
	OperFunc multiply;

	char32_t *(*to_string)(const struct Variable *);

	struct Type *next;
} Type;

struct Type TYPE_BOOL;
struct Type TYPE_INT;
struct Type TYPE_FLOAT;
struct Type TYPE_RUNE;
struct Type TYPE_STR;
struct Type TYPE_TYPE;

struct Type *TYPES_AVAILABLE;

bool make_new_type(const char *, size_t);

Type *find_type(const char *);

void free_types(void);
