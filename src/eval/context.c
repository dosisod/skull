#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"

#include "context.h"

/*
Add variable `var` to context `ctx`.

Returns `true` if `var` was added, else `false`
*/
bool context_add_var(context_t *ctx, variable_t *var) {
	if (context_find_name(ctx, var->name)!=NULL) {
		return false;
	}

	ctx->vars[ctx->vars_used]=var;
	ctx->vars_used++;

	return true;
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
variable_t *context_find_name(const context_t *ctx, const char32_t *name) {
	if (ctx==NULL) {
		return NULL;
	}

	size_t tmp=0;
	while (tmp < ctx->vars_used) {
		if (c32scmp(ctx->vars[tmp]->name, name)) {
			return ctx->vars[tmp];
		}
		tmp++;
	}

	if (ctx->parent) {
		return context_find_name(ctx->parent, name);
	}
	return NULL;
}

/*
Returns a new variable context.
*/
context_t *make_context(void) {
	context_t *ctx=calloc(1, sizeof(context_t));
	DIE_IF_MALLOC_FAILS(ctx);

	return ctx;
}

/*
Add another nested context to an existing context.
*/
void context_add_ctx(context_t *ctx, context_t *ctx_new) {
	ctx->child=ctx_new;
	ctx_new->parent=ctx;
}

/*
Frees a context `ctx` and all the variables inside of it.
*/
void free_context(context_t *ctx) {
	size_t tmp=0;

	while (tmp < ctx->vars_used) {
		free_variable(ctx->vars[tmp]);
		tmp++;
	}

	free(ctx);
}
