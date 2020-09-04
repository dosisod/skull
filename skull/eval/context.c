#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/eval/context.h"

/*
Add variable `var` to context `ctx`.

Returns `true` if `var` was added, else `false`
*/
bool context_add_var(Context *ctx, Variable *var) {
	if (context_find_name(ctx, var->name)) {
		return false;
	}

	ctx->vars[ctx->vars_used] = var;
	ctx->vars_used++;

	return true;
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
Variable *context_find_name(const Context *ctx, const char32_t *name) {
	if (!ctx) {
		return NULL;
	}

	size_t tmp = 0;
	while (tmp < ctx->vars_used) {
		if (c32scmp(ctx->vars[tmp]->name, name)) {
			return ctx->vars[tmp];
		}
		tmp++;
	}
	return NULL;
}

/*
Returns a new variable context.
*/
Context *make_context(void) {
	Context *ctx;
	ctx = calloc(1, sizeof *ctx);
	DIE_IF_MALLOC_FAILS(ctx);

	return ctx;
}

/*
Frees a context `ctx` and all the variables inside of it.
*/
void free_context(Context *ctx) {
	size_t tmp = 0;

	while (tmp < ctx->vars_used) {
		free_variable(ctx->vars[tmp]);
		tmp++;
	}

	free(ctx);
}
