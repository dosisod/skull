#pragma once

#include <stddef.h>

typedef struct Type {
	const char *name;
	size_t bytes;
} Type;

const Type TYPE_BOOL;
const Type TYPE_INT;
const Type TYPE_FLOAT;
const Type TYPE_RUNE;
const Type TYPE_STR;

const Type **TYPES_AVAILABLE;

const Type *find_type(const char *const);
