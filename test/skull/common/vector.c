#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/vector.h"

#include "test/skull/common/vector.h"
#include "test/testing.h"

TEST(vector_create_and_free, {
	Vector *v = make_vector();

	ASSERT_TRUTHY(v);
	ASSERT_TRUTHY(v->elements);
	ASSERT_TRUTHY(v->max);
	ASSERT_EQUAL(v->length, 0);

	free_vector(v, NULL);
})

TEST(vector_push_pop, {
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
})

TEST(vector_pop_empty, {
	Vector *v = make_vector();

	ASSERT_EQUAL(v->length, 0);
	ASSERT_FALSEY(vector_pop(v));
	ASSERT_EQUAL(v->length, 0);

	free_vector(v, NULL);
})

TEST(vector_scale, {
	Vector *v = make_vector();

	ASSERT_EQUAL(v->max, VECTOR_START_MAX);

	for (unsigned i = 0; i <= VECTOR_START_MAX; i++) {
		vector_push(v, NULL);
	}

	ASSERT_EQUAL(v->max, VECTOR_START_MAX * 2);

	free_vector(v, NULL);
})

TEST(vector_free_func, {
	Vector *v = make_vector();

	vector_push(v, Malloc(1));

	free_vector(v, free);
})

TEST_SELF(vector,
	test_vector_create_and_free,
	test_vector_push_pop,
	test_vector_pop_empty,
	test_vector_scale,
	test_vector_free_func
)
