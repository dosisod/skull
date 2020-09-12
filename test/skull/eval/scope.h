#include <stdbool.h>

#include "skull/eval/scope.h"

#include "test/testing.h"

TEST(make_scope, {
	Scope *scope=make_scope();

	const bool pass=(
		scope->vars_used==0 &&
		!scope->child
	);

	free(scope);

	return pass;
})

TEST(scope_find_name, {
	Scope *scope=make_scope();
	Variable *var=make_variable("int", U"x", true);

	scope_add_var(scope, var);

	const bool pass=(
		scope_find_name(scope, U"x")==var &&
		!scope_find_name(scope, U"y")
	);

	free(var);
	free(scope);

	return pass;
})

TEST(add_vars_to_scope, {
	Scope *scope=make_scope();
	Variable *var=make_variable("int", U"x", true);

	scope_add_var(scope, var);

	const bool pass=(
		scope->vars_used==1 &&
		scope->vars[0]==var
	);

	free(var);
	free(scope);

	return pass;
})

TEST(cannot_add_same_varname_to_scope, {
	Scope *scope=make_scope();
	Variable *var1=make_variable("int", U"x", true);
	Variable *var2=make_variable("int", U"x", true);

	const bool added_var1=scope_add_var(scope, var1);
	const bool added_var2=scope_add_var(scope, var2);

	const bool pass=(
		scope->vars_used==1 &&
		scope->vars[0]==var1 &&
		added_var1 &&
		!added_var2
	);

	free(var1);
	free(var2);
	free(scope);

	return pass;
})

TEST(free_scope, {
	Scope *scope=make_scope();
	Variable *var=make_variable("int", U"x", true);

	scope_add_var(scope, var);

	free_scope(scope);

	return true;
})

TEST(scope_find_name_when_null, {
	return !scope_find_name(NULL, U"anything");
})

void scope_test_self(bool *pass) {
	tests_t tests={
		test_make_scope,
		test_scope_find_name,
		test_add_vars_to_scope,
		test_cannot_add_same_varname_to_scope,
		test_free_scope,
		test_scope_find_name_when_null,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}