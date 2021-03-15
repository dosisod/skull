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
Return the element at index `index` of the vector `v`, or NULL if out-of-bound.
*/
void *vector_at(const Vector *const v, size_t index) {
	if (index >= v->length) {
		return NULL;
	}

	return v->elements[index];
}

/*
Free vector `v`, and any memory it might have allocated via `free_func`.
*/
void free_vector(Vector *v, void (*free_func)(void *)) {
	free_vector2(v, NULL, free_func);
}

/*
Free vector `v` using `free_func` to handle nested/managed objects
(ie, `HashTable`), and `free_func2` to free the data in those objects.
*/
void free_vector2(
	Vector *v,
	void (*free_func)(void (*)(void *), void *),
	void (*free_func2)(void *)
) {
	for RANGE(i, v->length) {
		if (free_func) {
			free_func(free_func2, v->elements[i]);
		}
		else if (free_func2) {
			free_func2(v->elements[i]);
		}
	}

	free(v->elements);
	v->elements = NULL;
	free(v);
}
