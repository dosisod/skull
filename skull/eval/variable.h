#pragma once

#include "skull/eval/types/types.h"

typedef struct Variable {
	Type *type;
	const char32_t *name;
	_Bool is_const;

	size_t bytes;
	unsigned char *mem;

	void *alloca; // only used in skullc for storing LLVMValueRef's
} Variable;

Variable *make_variable(const char *, const char32_t *, _Bool);

const char32_t *variable_write(const Variable *, const void *);
void variable_read(void *, const Variable *);

void free_variable(Variable *);

char32_t *fmt_var(const Variable *);