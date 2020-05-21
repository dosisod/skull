#pragma once

#include "variable.h"

#define CONTEXT_MAX_VARS 256

typedef struct context_t {
	size_t vars_used;
	variable_t *vars[CONTEXT_MAX_VARS];
} context_t;

/*
Returns a new variable context.
*/
context_t *make_context() {
	context_t *ctx=malloc(sizeof(context_t));

	ctx->vars_used=0;

	return ctx;
}

/*
Add variable `var` to context `ctx`.
*/
void context_add(context_t *ctx, variable_t *var) {
	size_t tmp=0;

	while (tmp < ctx->vars_used) {
		if (wcscmp(ctx->vars[tmp]->name, var->name)==0) {
			return;
		}
		tmp++;
	}

	ctx->vars[ctx->vars_used]=var;
	ctx->vars_used++;
}

/*
Frees a context `ctx` and all the variables inside of it.
*/
void free_context(context_t *ctx) {
	size_t tmp=0;

	while (tmp < ctx->vars_used) {
		free(ctx->vars[tmp]);
		tmp++;
	}

	free(ctx);
}
