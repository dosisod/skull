#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/vector.h"

#include "test/skull/common/vector.h"
#include "test/testing.h"

static bool test_vector_create_and_free(void) {
	Vector *v = make_vector();

	ASSERT_TRUTHY(v);
	ASSERT_TRUTHY(v->max);
	ASSERT_EQUAL(v->length, 0);

	free_vector(v, NULL);

	PASS
}

static bool test_vector_push_pop(void) {
	Vector *v = make_vector();

	ASSERT_EQUAL(v->length, 0);

	int x = 1234;
	vector_push(v, &x);
	ASSERT_EQUAL(v->length, 1);
	ASSERT_EQUAL(v->elements[0], &x);

	int *popped = vector_pop(v);
	ASSERT_EQUAL(popped, &x);
	ASSERT_EQUAL(v->length, 0);

	free_vector(v, NULL);

	PASS
}

static bool test_vector_pop_empty(void) {
	Vector *v = make_vector();

	ASSERT_EQUAL(v->length, 0);
	ASSERT_FALSEY(vector_pop(v));
	ASSERT_EQUAL(v->length, 0);

	free_vector(v, NULL);

	PASS
}

static bool test_vector_scale(void) {
	Vector *v = make_vector();

	ASSERT_EQUAL(v->max, VECTOR_START_MAX);

	for (unsigned i = 0; i <= VECTOR_START_MAX; i++) {
		vector_push(v, NULL);
	}

	ASSERT_EQUAL(v->max, (unsigned long)VECTOR_START_MAX * 2);

	free_vector(v, NULL);

	PASS
}

static bool test_vector_free_func(void) {
	Vector *v = make_vector();

	vector_push(v, Malloc(1));

	free_vector(v, free);

	PASS
}

static bool test_vector_at(void) {
	Vector *v = make_vector();

	int i = 1234;
	vector_push(v, &i);

	ASSERT_EQUAL(vector_at(v, 0), &i);
	ASSERT_EQUAL(vector_at(v, 9999), NULL);

	free_vector(v, NULL);

	PASS
}

static bool test_vector_freeze(void) {
	Vector *v = make_vector();

	int i = 1234;
	vector_push(v, &i);

	int **ints = vector_freeze(v);

	ASSERT_EQUAL(ints[0], &i);

	ASSERT_EQUAL(v->length, 1);
	vector_push(v, &i);
	ASSERT_EQUAL(v->length, 1);

	ASSERT_FALSEY(vector_pop(v));

	free_vector(v, NULL);
	free(ints);

	PASS
}

void vector_test_self(bool *pass) {
	RUN_ALL(
		test_vector_create_and_free,
		test_vector_push_pop,
		test_vector_pop_empty,
		test_vector_scale,
		test_vector_free_func,
		test_vector_at,
		test_vector_freeze
	)
}
