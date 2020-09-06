#pragma once

#include "skull/eval/types/types.h"

typedef struct Variable {
	Type *type;
	const char32_t *name;
	bool is_const;

	size_t bytes;
	unsigned char *mem;
} Variable;

Variable *make_variable(const char *, const char32_t *, bool);

const char32_t *variable_write(const Variable *, const void *);
void variable_read(void *, const Variable *);

void free_variable(Variable *);

char32_t *fmt_var(const Variable *);