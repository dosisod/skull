#include <stdbool.h>

#include "skull/eval/scope.h"

#include "test/skull/eval/scope.h"
#include "test/testing.h"

TEST(make_scope, {
	Scope *scope = make_scope();

	ASSERT_EQUAL(scope->vars_used, 0);

	free(scope);
})

TEST(scope_find_name, {
	Scope *scope = make_scope();
	Variable *var = make_variable("int", U"x", true);

	scope_add_var(scope, var);

	ASSERT_EQUAL(scope_find_name(scope, U"x"), var);
	ASSERT_FALSEY(scope_find_name(scope, U"y"));

	free(var);
	free(scope);
})

TEST(add_vars_to_scope, {
	Scope *scope = make_scope();
	Variable *var = make_variable("int", U"x", true);

	scope_add_var(scope, var);

	ASSERT_EQUAL(scope->vars_used, 1);
	ASSERT_EQUAL(scope->vars[0], var);

	free(var);
	free(scope);
})

TEST(cannot_add_same_varname_to_scope, {
	Scope *scope = make_scope();
	Variable *var1 = make_variable("int", U"x", true);
	Variable *var2 = make_variable("int", U"x", true);

	ASSERT_TRUTHY(scope_add_var(scope, var1));
	ASSERT_FALSEY(scope_add_var(scope, var2));

	ASSERT_EQUAL(scope->vars_used, 1);
	ASSERT_EQUAL(scope->vars[0], var1);

	free(var1);
	free(var2);
	free(scope);
})

TEST(free_scope, {
	Scope *scope = make_scope();
	Variable *var = make_variable("int", U"x", true);

	scope_add_var(scope, var);

	free_scope(scope);
})

TEST(scope_find_name_when_null, {
	ASSERT_FALSEY(scope_find_name(NULL, U"anything"));
})

TEST_SELF(scope,
	test_make_scope,
	test_scope_find_name,
	test_add_vars_to_scope,
	test_cannot_add_same_varname_to_scope,
	test_free_scope,
	test_scope_find_name_when_null
)