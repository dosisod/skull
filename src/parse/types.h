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

struct type_t *TYPES_AVAILABLE;

/*
Creates a new type named `type` that allocates `bytes` bytes.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char32_t *name, size_t bytes);

/*
Returns pointer to type with name `name`.
*/
type_t *find_type(const char32_t *name);

/*
Free all non-builtin types.
*/
void free_types(void);
