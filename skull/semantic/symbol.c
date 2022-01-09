#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/errors.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/symbol.h"

static Symbol find_symbol(char *);

static bool symbol_already_exists(Symbol *new_symbol) {
	Symbol symbol = find_symbol(new_symbol->name);

	const Location *location = new_symbol->location;
	char *name = new_symbol->name;

	if (symbol.type == SYMBOL_VAR) {
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
	else if (symbol.type == SYMBOL_ALIAS) {
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
	else if (symbol.type == SYMBOL_FUNC) {
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

static Symbol find_symbol(char *name) {
	Symbol *symbol = scope_find_name(SEMANTIC_STATE.scope, name);
	if (symbol && symbol->type == SYMBOL_VAR) {
		return (Symbol){
			.name = name,
			.expr_type = symbol->var->type,
			.type = SYMBOL_VAR,
			.var = symbol->var
		};
	}

	FunctionDeclaration *func = find_func_by_name(name);
	if (func) {
		return (Symbol){
			.name = name,
			.expr_type = func->return_type,
			.type = SYMBOL_FUNC,
			.func = func
		};
	}

	Type *type = find_type(name);
	if (type) {
		return (Symbol){
			.name = name,
			.expr_type = type,
			.type = SYMBOL_ALIAS
		};
	}

	return (Symbol){0};
}

bool scope_add_symbol(Symbol *symbol) {
	if (symbol_already_exists(symbol)) {
		return false;
	}

	if (!SEMANTIC_STATE.scope) SEMANTIC_STATE.scope = make_scope();
	if (!SEMANTIC_STATE.scope->symbols)
		SEMANTIC_STATE.scope->symbols = make_ht();

	return ht_add(SEMANTIC_STATE.scope->symbols, symbol->name, symbol);
}

