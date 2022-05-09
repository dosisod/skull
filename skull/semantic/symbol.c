#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/errors.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/symbol.h"

static SymbolType find_symbol_type(SemanticState *, char *);

static bool symbol_already_exists(SemanticState *state, Symbol *new_symbol) {
	SymbolType type = find_symbol_type(state, new_symbol->name);
	if (type == SYMBOL_UNKNOWN) return false;

	const Location *location = &new_symbol->location;
	char *name = new_symbol->name;

	if (type == SYMBOL_VAR) {
		if (new_symbol->type == SYMBOL_VAR) {
			FMT_ERROR(ERR_VAR_ALREADY_DEFINED, {
				.loc = location,
				.real = name
			});

			return true;
		}

		FMT_ERROR(
			new_symbol->type == SYMBOL_FUNC ?
				ERR_NO_REDEFINE_VAR_AS_FUNC :
				ERR_NO_REDEFINE_VAR_AS_ALIAS,
			{ .loc = location, .real = name }
		);
	}
	else if (type == SYMBOL_ALIAS) {
		if (new_symbol->type == SYMBOL_ALIAS) {
			FMT_ERROR(ERR_ALIAS_ALREADY_DEFINED, {
				.loc = location,
				.real = name
			});

			return true;
		}

		FMT_ERROR(
			new_symbol->type == SYMBOL_VAR ?
				ERR_NO_REDEFINE_ALIAS_AS_VAR :
				ERR_NO_REDEFINE_ALIAS_AS_FUNC,
			{ .loc = location, .real = name }
		);
	}
	else if (type == SYMBOL_FUNC) {
		if (new_symbol->type == SYMBOL_FUNC) {
			FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
				.loc = location,
				.real = name
			});

			return true;
		}

		FMT_ERROR(
			new_symbol->type == SYMBOL_ALIAS ?
				ERR_NO_REDEFINE_FUNC_AS_ALIAS :
				ERR_NO_REDEFINE_FUNC_AS_VAR,
			{ .loc = location, .real = name }
		);
	}
	else return false;

	return true;
}

static SymbolType find_symbol_type(SemanticState *state, char *name) {
	Symbol *symbol = scope_find_name(state->scope, name);
	if (symbol && symbol->type == SYMBOL_VAR) return SYMBOL_VAR;

	symbol = find_func_by_name(state, name);
	if (symbol) return SYMBOL_FUNC;

	const Type *type = find_type(state, name);
	if (type) return SYMBOL_ALIAS;

	return SYMBOL_UNKNOWN;
}

bool scope_add_symbol(SemanticState *state, Symbol *symbol) {
	if (!state || symbol_already_exists(state, symbol)) {
		return false;
	}

	if (!state->scope) state->scope = make_scope();
	if (!state->scope->symbols)
		state->scope->symbols = make_ht();

	return ht_add(state->scope->symbols, symbol->name, symbol);
}
