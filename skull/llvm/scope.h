#pragma once

#include "skull/eval/variable.h"
#include "skull/llvm/shared.h"
#include "skull/parse/token.h"

/*
Make new scope and set the current scope to be a sub-scope of the new one.
*/
#define MAKE_SUB_SCOPE \
	Scope *scope_copy = SKULL_STATE.scope; \
	SKULL_STATE.scope = make_scope(); \
	SKULL_STATE.scope->sub_scope = scope_copy

/*
Free the new scope, set the current scope to the old sub-scope.
*/
#define RESTORE_SUB_SCOPE \
	free_scope(SKULL_STATE.scope); \
	SKULL_STATE.scope = scope_copy

Variable *scope_find_var(const Token *const);
