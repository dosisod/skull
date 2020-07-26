#pragma once

#include "skull/eval/variable.h"

#define CONTEXT_MAX_VARS 256

typedef struct context_t {
	struct context_t *child;
	struct context_t *parent;

	size_t vars_used;
	variable_t *vars[CONTEXT_MAX_VARS];
} context_t;

context_t *make_context(void);

variable_t *context_find_name(const context_t *, const char32_t *);
bool context_contains_var(context_t *, const variable_t *);
bool context_add_var(context_t *, variable_t *);
void context_add_ctx(context_t *, context_t *);

void free_context(context_t *);
