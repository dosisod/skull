#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/compiler/types.h"

#include "test/skull/compiler/types/types.h"
#include "test/testing.h"

bool test_find_type() {
	ASSERT_EQUAL(find_type("Int"), TYPE_INT);

	PASS
}

void types_test_self(bool *pass) {
	RUN_ALL(
		test_find_type
	)
}
