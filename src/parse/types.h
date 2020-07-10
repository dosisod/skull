#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>

typedef struct type_t {
	const char32_t *name;
	size_t bytes;

	struct type_t *next;
} type_t;

struct type_t TYPE_BOOL;
struct type_t TYPE_INT;
struct type_t TYPE_FLOAT;
struct type_t TYPE_CHAR;
struct type_t TYPE_STR;
struct type_t TYPE_TEMPLATE;

struct type_t *TYPES_AVAILABLE;

bool make_new_type(const char32_t *, size_t);

type_t *find_type(const char32_t *);

void free_types(void);
