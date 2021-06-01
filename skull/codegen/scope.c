#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/compiler/scope.h"

#include "skull/codegen/scope.h"

/*
Try and find a variable stored in `token`.

Return `NULL` if variable was not found.
*/
Variable *scope_find_var(const Token *const token) {
	char *const var_name = token_mbs_str(token);
	Variable *const var = scope_find_name(SKULL_STATE.scope, var_name);

	if (!var || !var->ref) {
		FMT_ERROR(ERR_VAR_NOT_FOUND, {
			.loc = &token->location, .real = var_name
		});

		return NULL;
	}
	free(var_name);

	return var;
}

/*
Make new scope and set the current scope to be a sub-scope of the new one.
*/
void make_sub_scope(Scope **old, Scope **new) {
	*new = *old;
	if (!*new) *new = make_scope();
	*old = make_scope();
	(*old)->sub_scope = *new;
}

/*
Free the new scope, set the current scope to the old sub-scope.
*/
void restore_sub_scope(Scope **old, Scope **new) {
	free_scope(*old);
	*old = *new;
}
