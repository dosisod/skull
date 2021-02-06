#pragma once

#include <uchar.h>

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"

/*
`Variable` acts as a more strict abstraction on top of LLVM.

`type` is the Skull type of the variable.

`name` is the name of the variable.

`is_const` stores whether the variable is const or not.

`is_const_lit` stores whether the variable is const literal.

`is_global` stores whether the variable was declared at the top level of a
module.

`llvm_value` stores the actual LLVM alloca/constant/global.
*/
typedef struct Variable {
	const Type *type;
	char *name;
	_Bool is_const : 1;
	_Bool is_const_lit : 1;
	_Bool is_global : 1;

	LLVMValueRef llvm_value;
} Variable;

Variable *make_variable(const Type *const, const char32_t *const, _Bool);

void free_variable(Variable *);
