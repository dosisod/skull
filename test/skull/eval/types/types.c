#include <stdlib.h>
#include <string.h>

#include "skull/eval/types/types.h"

#include "test/skull/eval/types/types.h"
#include "test/testing.h"

TEST(find_type, {
	ASSERT_EQUAL(find_type("Int"), &TYPE_INT);
})

_Bool mangle_types_fixture(
	const Type **type,
	unsigned num_types,
	char ident,
	const char *expected
) {
	char *mangled = mangle_types(type, num_types, ident);

	const _Bool ret = strcmp(mangled, expected) == 0;
	free(mangled);

	return ret;
}

TEST(type_mangle_types, {
	ASSERT_TRUTHY(mangle_types_fixture(
		(const Type*[]){ &TYPE_INT, &TYPE_INT }, 2, 'X',
		"_X<Int,Int>"
	));

	ASSERT_TRUTHY(mangle_types_fixture(
		(const Type*[]){ &TYPE_INT }, 1, 'X',
		"_X<Int>"
	));
})

TEST_SELF(types,
	test_find_type,
	test_type_mangle_types
)
