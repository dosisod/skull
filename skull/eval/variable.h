#pragma once

#include <uchar.h>

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"

/*
`Variable` acts as a more strict abstraction on top of LLVM.

`type` is the Skull type of the variable.

`name` is the name of the variable.

`is_const` stores whether the variable is const or not.

`alloca` stores the actual LLVM allocated variable.
*/
typedef struct Variable {
	const Type *type;
	char *name;
	_Bool is_const;

	LLVMValueRef alloca;
} Variable;

Variable *make_variable(const Type *const, const char32_t *const, _Bool);

void free_variable(Variable *);