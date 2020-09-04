#include <stdbool.h>

#include "skull/eval/context.h"

#include "test/testing.h"

TEST(make_context, {
	Context *ctx=make_context();

	const bool pass=(
		ctx->vars_used==0 &&
		!ctx->child
	);

	free(ctx);

	return pass;
})

TEST(context_find_name, {
	Context *ctx=make_context();
	Variable *var=make_variable("int", U"x", true);

	context_add_var(ctx, var);

	const bool pass=(
		context_find_name(ctx, U"x")==var &&
		!context_find_name(ctx, U"y")
	);

	free(var);
	free(ctx);

	return pass;
})

TEST(add_vars_to_context, {
	Context *ctx=make_context();
	Variable *var=make_variable("int", U"x", true);

	context_add_var(ctx, var);

	const bool pass=(
		ctx->vars_used==1 &&
		ctx->vars[0]==var
	);

	free(var);
	free(ctx);

	return pass;
})

TEST(cannot_add_same_varname_to_context, {
	Context *ctx=make_context();
	Variable *var1=make_variable("int", U"x", true);
	Variable *var2=make_variable("int", U"x", true);

	const bool added_var1=context_add_var(ctx, var1);
	const bool added_var2=context_add_var(ctx, var2);

	const bool pass=(
		ctx->vars_used==1 &&
		ctx->vars[0]==var1 &&
		added_var1 &&
		!added_var2
	);

	free(var1);
	free(var2);
	free(ctx);

	return pass;
})

TEST(free_context, {
	Context *ctx=make_context();
	Variable *var=make_variable("int", U"x", true);

	context_add_var(ctx, var);

	free_context(ctx);

	return true;
})

TEST(context_find_name_when_null, {
	return !context_find_name(NULL, U"anything");
})

void Contextest_self(bool *pass) {
	tests_t tests={
		test_make_context,
		test_context_find_name,
		test_add_vars_to_context,
		test_cannot_add_same_varname_to_context,
		test_free_context,
		test_context_find_name_when_null,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}