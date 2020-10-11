#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/eval_assign.h"
#include "test/testing.h"

TEST(eval_assign_string_types_cannot_share_pointers, {
	Variable *var1 = make_variable(&TYPE_STR, U"var1", false);
	const char32_t *str1 = NULL;
	variable_write(var1, &str1);

	Variable *var2 = make_variable(&TYPE_STR, U"var2", false);
	const char32_t *str2 = c32sdup(U"anything");
	variable_write(var2, &str2);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"var2", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output = eval_assign(var1, node, scope);
	ASSERT_FALSEY(output);

	char32_t *after_var1 = NULL;
	variable_read(&after_var1, var1);
	ASSERT_TRUTHY(c32scmp(after_var1, str2));

	char32_t *after_var2 = NULL;
	variable_read(&after_var2, var2);
	ASSERT_TRUTHY(c32scmp(after_var1, str2));

	ASSERT_NOT_EQUAL(after_var1, after_var2);

	free_scope(scope);
})

TEST_SELF(eval_assign,
	test_eval_assign_string_types_cannot_share_pointers
)
