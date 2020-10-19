#pragma once

#include <uchar.h>

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"

typedef struct Variable {
	const Type *type;
	char32_t *name;
	_Bool is_const;

	LLVMValueRef alloca;
} Variable;

Variable *make_variable(const Type *const, const char32_t *const, _Bool);

void free_variable(Variable *);