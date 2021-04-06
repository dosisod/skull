#pragma once

#include <stddef.h>

#define VECTOR_START_MAX 8

#define MAKE_VECTOR(type) \
	void vector_push_##type(Vector *v, type *ptr) { /* NOLINT */ \
		vector_push(v, (void *)ptr); \
	} \
	type *vector_pop_##type(Vector *v) { /* NOLINT */ \
		return (type *)vector_pop(v); \
	} \
	type *vector_at_##type(const Vector *const v, size_t index) { /* NOLINT */\
		return (type *)vector_at(v, index); \
	}

/*
Simple Vector implementation.

`length` stores the length of the vector.

`max` stores the max length needed before reallocation.

`elements` stores the actual elements of the vector.

`is_array` indicates that the vector has been converted into an array, and
should not be used further.
*/
typedef struct {
	size_t length;
	size_t max;
	void **elements;
	_Bool is_array : 1;
} Vector;

Vector *make_vector(void);

void *vector_freeze(Vector *);

void vector_push(Vector *, void *);
void *vector_pop(Vector *);

#define VECTOR_AT(v, index, type) ((type)vector_at((v), (index)))

void *vector_at(const Vector *const, size_t);

void free_vector(Vector *, void (*)(void *));
void free_vector2(
	Vector *,
	void (*)(void (*)(void *), void *),
	void (*)(void *)
);
