#pragma once

#include "skull/eval/types/types.h"

typedef struct Variable {
	const Type *type;
	const char32_t *name;
	_Bool is_const;

	unsigned char *mem;

	void *alloca; // only used to store LLVMValueRef's
} Variable;

Variable *make_variable(const Type *const, const char32_t *const, _Bool);

const char32_t *variable_write(const Variable *const, const void *const);
void variable_read(void *const, const Variable *const);

void free_variable(Variable *);

char32_t *fmt_var(const Variable *const);