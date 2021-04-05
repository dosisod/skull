#include <stdbool.h>
#include <stdlib.h>

#include "skull/compiler/scope.h"

#include "test/skull/compiler/scope.h"
#include "test/testing.h"

TEST(make_scope, {
	Scope *scope = make_scope();

	free_scope(scope);
})

TEST(scope_find_name, {
	Scope *scope = make_scope();
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	scope_add_var(scope, var);

	ASSERT_EQUAL(scope_find_name(scope, "x"), var);
	ASSERT_FALSEY(scope_find_name(scope, "y"));

	free_scope(scope);
})

TEST(add_vars_to_scope, {
	Scope *scope = make_scope();
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	scope_add_var(scope, var);

	ASSERT_EQUAL(ht_get(scope->vars, "x"), var);

	free_scope(scope);
})

TEST(cannot_add_same_varname_to_scope, {
	Scope *scope = make_scope();
	Variable *var1 = make_variable(&TYPE_INT, U"x", true);
	Variable *var2 = make_variable(&TYPE_INT, U"x", true);

	ASSERT_TRUTHY(scope_add_var(scope, var1));
	ASSERT_FALSEY(scope_add_var(scope, var2));

	ASSERT_EQUAL(ht_get(scope->vars, "x"), var1);

	free_variable(var2);
	free_scope(scope);
})

TEST(free_scope, {
	Scope *scope = make_scope();
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	scope_add_var(scope, var);

	free_scope(scope);
})

TEST(scope_find_name_when_null, {
	ASSERT_FALSEY(scope_find_name(NULL, "anything"));
})

TEST_SELF(scope,
	test_make_scope,
	test_scope_find_name,
	test_add_vars_to_scope,
	test_cannot_add_same_varname_to_scope,
	test_free_scope,
	test_scope_find_name_when_null
)