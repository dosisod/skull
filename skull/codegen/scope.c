#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/eval/scope.h"

#include "skull/codegen/scope.h"

/*
Try and find a variable stored in `token`.
*/
Variable *scope_find_var(const Token *const token) {
	char *const var_name = token_mbs_str(token);
	Variable *const var = scope_find_name(SKULL_STATE.scope, var_name);

	if (!var) {
		PANIC(ERR_VAR_NOT_FOUND, { .real = var_name });
	}
	free(var_name);

	return var;
}
