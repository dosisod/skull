#pragma once

#include <stddef.h>

#define VECTOR_START_MAX 8

/*
Simple Vector implementation.

`length` stores the length of the vector.

`max` stores the max length needed before reallocation.

`elements` stores the actual elements.
*/
typedef struct {
	size_t length;
	size_t max;
	void **elements;
} Vector;

Vector *make_vector(void);

void vector_push(Vector *v, void *);
void *vector_pop(Vector *v);

void free_vector(Vector *v, void (*)(void *));
