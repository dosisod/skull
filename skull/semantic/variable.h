#pragma once

#include <uchar.h>

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/parse/token.h"
#include "skull/semantic/types.h"

/*
`Variable` acts as a more strict abstraction on top of LLVM.

`type` is the Skull type of the variable.

`name` is the name of the variable.

`is_const` stores whether the variable is const or not.

`is_const_lit` stores whether the variable is const literal.

`is_global` stores whether the variable was declared at the top level of a
module.

`was_reassigned` stores whether a (mutable) variable was reassigned.

`was_read` stores whether a variable was read from or not.

`implicitly_typed` is true when variable type is auto-deduced.

`location` stores the location where the variable was first defined.

`ref` stores the actual LLVM alloca/constant/global.
*/
typedef struct Variable {
	Type type;
	char *name;
	_Bool is_const : 1;
	_Bool is_const_lit : 1;
	_Bool is_global : 1;
	_Bool was_reassigned : 1;
	_Bool was_read : 1;
	_Bool implicitly_typed : 1;
	_Bool is_exported : 1;

	Location location;

	LLVMValueRef ref;
} Variable;

Variable *make_variable(Type, const char32_t *const, _Bool);

void free_variable(Variable *);

void variable_no_warnings(Variable *);
