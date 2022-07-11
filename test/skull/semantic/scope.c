#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "test/skull/semantic/scope.h"
#include "test/testing.h"

static Symbol *symbol_wrapper(Variable *var) {
	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);

	*symbol = (Symbol){
		.name = var->name,
		.expr_type = var->type,
		.type = SYMBOL_VAR,
		.var = var
	};

	return symbol;
}

static Symbol *make_namespace_symbol(const char *name) {
	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);

	*symbol = (Symbol) {
		.name = strdup(name),
		.type = SYMBOL_NAMESPACE
	};

	return symbol;
}

static bool test_make_scope(void) {
	Scope *scope = make_scope();

	free_scope(scope);

	PASS
}

static bool test_scope_find_name(void) {
	SemanticState *state = setup_semantic_state();

	state->scope = make_scope();
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	scope_add_var(state, symbol_wrapper(var));

	ASSERT_EQUAL(scope_find_name(state->scope, (char[]){"x"})->var, var);
	ASSERT_FALSEY(scope_find_name(state->scope, (char[]){"y"}));

	free_semantic_state(state);

	PASS
}

static bool test_add_vars_to_scope(void) {
	SemanticState *state = setup_semantic_state();

	state->scope = make_scope();
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	scope_add_var(state, symbol_wrapper(var));

	ASSERT_EQUAL(((Symbol *)ht_get(state->scope->symbols, "x"))->var, var);

	free_semantic_state(state);

	PASS
}

static bool test_cannot_add_same_varname_to_scope(void) {
	SemanticState *state = setup_semantic_state();

	state->scope = make_scope();
	Variable *var1 = make_variable(&TYPE_INT, U"x", true);
	Variable *var2 = make_variable(&TYPE_INT, U"x", true);

	ASSERT_TRUTHY(scope_add_var(state, symbol_wrapper(var1)));

	Symbol *temp = symbol_wrapper(var2);
	ASSERT_FALSEY(scope_add_var(state, temp));
	free(temp);

	ASSERT_EQUAL(((Symbol *)ht_get(state->scope->symbols, "x"))->var, var1);

	free_variable(var2);
	free_semantic_state(state);

	PASS
}

static bool test_scope_find_name_when_null(void) {
	ASSERT_FALSEY(scope_find_name(NULL, (char[]){"anything"}));

	PASS
}

static bool test_scope_find_name_in_namespace(void) {
	SemanticState *state = setup_semantic_state();

	Scope *global_scope = make_scope();
	Scope *namespace_scope = make_scope();
	state->scope = global_scope;
	state->scope->child = namespace_scope;

	Symbol *namespace = make_namespace_symbol("x");
	namespace->scope = namespace_scope;
	Symbol *variable = symbol_wrapper(make_variable(&TYPE_INT, U"y", true));

	ASSERT_TRUTHY(scope_add_symbol(state, namespace));
	state->scope = namespace_scope;

	ASSERT_TRUTHY(scope_add_symbol(state, variable));
	state->scope = global_scope;

	char *name = (char[]){"x.y"};
	Symbol *found_var = scope_find_name(global_scope, name);

	ASSERT_TRUTHY(found_var);
	ASSERT_EQUAL(found_var, variable);
	ASSERT_EQUAL(strcmp(name, "x.y"), 0);

	free_semantic_state(state);

	PASS
}

void scope_test_self(bool *pass) {
	RUN_ALL(
		test_make_scope,
		test_scope_find_name,
		test_add_vars_to_scope,
		test_cannot_add_same_varname_to_scope,
		test_scope_find_name_when_null,
		test_scope_find_name_in_namespace
	)
}
