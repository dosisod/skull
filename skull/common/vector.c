#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/common/vector.h"

/*
Push pointer `ptr` to vector `v`. Scale vector if needed.
*/
void vector_push(Vector *v, void *ptr) {
	if (v->length >= v->max) {
		v->max *= 2;
		v->elements = Realloc(v->elements, v->max * sizeof(void *));
	}

	v->elements[v->length] = ptr;
	v->length++;
}

/*
Pop and return last element from vector `v`.
*/
void *vector_pop(Vector *v) {
	if (!v->length) {
		return NULL;
	}

	v->length--;
	return v->elements[v->length];
}

/*
Return new vector.
*/
Vector *make_vector(void) {
	Vector *v;
	v = Calloc(1, sizeof *v);
	v->max = VECTOR_START_MAX;

	v->elements = Calloc(VECTOR_START_MAX, sizeof(void *));

	return v;
}

/*
Free vector `v`.
*/
void free_vector(Vector *v, void (*free_func)(void *)) {
	if (free_func) {
		for RANGE(i, v->length) {
			free_func(v->elements[i]);
		}
	}

	free(v->elements);
	v->elements = NULL;
	free(v);
}
