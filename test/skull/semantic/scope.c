#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "test/skull/semantic/scope.h"
#include "test/testing.h"

Symbol *symbol_wrapper(Variable *var) {
	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);

	*symbol = (Symbol){
		.name = var->name,
		.expr_type = var->type,
		.location = &var->location,
		.type = SYMBOL_VAR,
		.var = var
	};

	return symbol;
}

bool test_make_scope() {
	Scope *scope = make_scope();

	free_scope(scope);

	PASS
}

bool test_scope_find_name() {
	SEMANTIC_STATE.scope = make_scope();
	Variable *var = make_variable(TYPE_INT, U"x", true);

	scope_add_var(symbol_wrapper(var));

	ASSERT_EQUAL(scope_find_name(SEMANTIC_STATE.scope, "x")->var, var);
	ASSERT_FALSEY(scope_find_name(SEMANTIC_STATE.scope, "y"));

	free_scope(SEMANTIC_STATE.scope);

	PASS
}

bool test_add_vars_to_scope() {
	SEMANTIC_STATE.scope = make_scope();
	Variable *var = make_variable(TYPE_INT, U"x", true);

	scope_add_var(symbol_wrapper(var));

	ASSERT_EQUAL(((Symbol *)ht_get(SEMANTIC_STATE.scope->symbols, "x"))->var, var);

	free_scope(SEMANTIC_STATE.scope);

	PASS
}

bool test_cannot_add_same_varname_to_scope() {
	SEMANTIC_STATE.scope = make_scope();
	Variable *var1 = make_variable(TYPE_INT, U"x", true);
	Variable *var2 = make_variable(TYPE_INT, U"x", true);

	ASSERT_TRUTHY(scope_add_var(symbol_wrapper(var1)));

	Symbol *temp = symbol_wrapper(var2);
	ASSERT_FALSEY(scope_add_var(temp));
	free(temp);

	ASSERT_EQUAL(((Symbol *)ht_get(SEMANTIC_STATE.scope->symbols, "x"))->var, var1);

	free_variable(var2);
	free_scope(SEMANTIC_STATE.scope);

	PASS
}

bool test_free_scope() {
	SEMANTIC_STATE.scope = make_scope();
	Variable *var = make_variable(TYPE_INT, U"x", true);

	scope_add_var(symbol_wrapper(var));

	free_scope(SEMANTIC_STATE.scope);

	PASS
}

bool test_scope_find_name_when_null() {
	ASSERT_FALSEY(scope_find_name(NULL, "anything"));

	PASS
}

void scope_test_self(bool *pass) {
	RUN_ALL(
		test_make_scope,
		test_scope_find_name,
		test_add_vars_to_scope,
		test_cannot_add_same_varname_to_scope,
		test_free_scope,
		test_scope_find_name_when_null
	)
}
