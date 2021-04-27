#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/compiler/types/types.h"

#include "test/skull/compiler/types/types.h"
#include "test/testing.h"

bool test_find_type() {
	ASSERT_EQUAL(find_type("Int"), TYPE_INT);

	PASS
}

_Bool mangle_types_fixture(
	Type *type,
	unsigned num_types,
	char ident,
	const char *expected
) {
	char *mangled = mangle_types(type, num_types, ident);

	const _Bool ret = strcmp(mangled, expected) == 0;
	free(mangled);

	return ret;
}

bool test_type_mangle_types() {
	ASSERT_TRUTHY(mangle_types_fixture(
		(Type[]){ TYPE_INT, TYPE_INT }, 2, 'X',
		"_X<Int,Int>"
	));

	ASSERT_TRUTHY(mangle_types_fixture(
		(Type[]){ TYPE_INT }, 1, 'X',
		"_X<Int>"
	));

	PASS
}

void types_test_self(bool *pass) {
	RUN_ALL(
		test_find_type,
		test_type_mangle_types
	)
}
