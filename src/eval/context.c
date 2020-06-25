#include <stddef.h>
#include <stdlib.h>

#include "../common/malloc.h"
#include "../common/str.h"

#include "context.h"

context_t *make_context(void) {
	context_t *ctx=malloc(sizeof(context_t));
	DIE_IF_MALLOC_FAILS(ctx);

	ctx->vars_used=0;
	ctx->child=NULL;
	ctx->parent=NULL;

	return ctx;
}

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

bool context_contains_var(context_t *ctx, const variable_t *var) {
	size_t tmp=0;

	while (tmp < ctx->vars_used) {
		if (c32scmp(ctx->vars[tmp]->name, var->name)) {
			return true;
		}
		tmp++;
	}

	if (ctx->parent) {
		return context_contains_var(ctx->parent, var);
	}
	return false;
}

bool context_add_var(context_t *ctx, variable_t *var) {
	if (context_contains_var(ctx, var)) {
		return false;
	}

	ctx->vars[ctx->vars_used]=var;
	ctx->vars_used++;

	return true;
}

void context_add_ctx(context_t *ctx, context_t *ctx_new) {
	ctx->child=ctx_new;
	ctx_new->parent=ctx;
}

void free_context(context_t *ctx) {
	size_t tmp=0;

	while (tmp < ctx->vars_used) {
		free_variable(ctx->vars[tmp]);
		tmp++;
	}

	free(ctx);
}
