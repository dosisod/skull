#include "skull/eval/types/types.h"

#include "test/skull/eval/types/types.h"
#include "test/testing.h"

TEST(find_type, {
	ASSERT_EQUAL(find_type("int"), &TYPE_INT);
})

TEST_SELF(types, test_find_type)
