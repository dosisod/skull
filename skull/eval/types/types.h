#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>

struct variable_t;

typedef struct type_t {
	const char32_t *name;
	size_t bytes;

	struct variable_t *(*add)(const struct variable_t *, const struct variable_t *);
	struct variable_t *(*subtract)(const struct variable_t *, const struct variable_t *);
	struct variable_t *(*divide)(const struct variable_t *, const struct variable_t *);
	struct variable_t *(*multiply)(const struct variable_t *, const struct variable_t *);

	char32_t *(*to_string)(const struct variable_t *);

	struct type_t *next;
} type_t;

struct type_t TYPE_BOOL;
struct type_t TYPE_INT;
struct type_t TYPE_FLOAT;
struct type_t TYPE_RUNE;
struct type_t TYPE_STR;
struct type_t TYPE_TYPE;

struct type_t *TYPES_AVAILABLE;

bool make_new_type(const char32_t *, size_t);

type_t *find_type(const char32_t *);

void free_types(void);
