#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/compiler/types.h"
#include "skull/compiler/variable.h"

#include "skull/semantic/symbol.h"

bool is_valid_symbol(
	const Location *location,
	char *name,
	SymbolType type
) {
	if (type != SYMBOL_VAR && scope_find_name(SKULL_STATE.scope, name)) {
		FMT_ERROR(
			type == SYMBOL_FUNC ?
				ERR_NO_REDEFINE_VAR_AS_FUNC :
				ERR_NO_REDEFINE_VAR_AS_ALIAS,
			{ .loc = location, .real = name }
		);
	}
	else if (type != SYMBOL_ALIAS && find_type(name)) {
		FMT_ERROR(
			type == SYMBOL_VAR ?
				ERR_NO_REDEFINE_ALIAS_AS_VAR :
				ERR_NO_REDEFINE_ALIAS_AS_FUNC,
			{ .loc = location, .real = name }
		);
	}
	else if (type != SYMBOL_FUNC && find_func_by_name(name)) {
		FMT_ERROR(
			type == SYMBOL_ALIAS ?
				ERR_NO_REDEFINE_FUNC_AS_ALIAS :
				ERR_NO_REDEFINE_FUNC_AS_VAR,
			{ .loc = location, .real = name }
		);
	}
	else return true;

	return false;
}
