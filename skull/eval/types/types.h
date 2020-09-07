#pragma once

#include <stdbool.h>
#include <uchar.h>

struct Variable;

typedef struct Type {
	const char *name;
	size_t bytes;

	struct Variable *(*add)(const struct Variable *, const struct Variable *);
	struct Variable *(*subtract)(const struct Variable *, const struct Variable *);
	struct Variable *(*divide)(const struct Variable *, const struct Variable *);
	struct Variable *(*multiply)(const struct Variable *, const struct Variable *);

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
