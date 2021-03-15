#pragma once

#include <stddef.h>

#define VECTOR_START_MAX 8

/*
Simple Vector implementation.

`length` stores the length of the vector.

`max` stores the max length needed before reallocation.

`elements` stores the actual elements of the vector.
*/
typedef struct {
	size_t length;
	size_t max;
	void **elements;
} Vector;

Vector *make_vector(void);

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
