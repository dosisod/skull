#pragma once

#include <stddef.h>

#include <llvm-c/Core.h>

#include "skull/common/hashtable.h"

/*
`Type` is an abstraction on top of the existing LLVM type system.

`name` is the name of the type.

`llvm_type` is a pointer to a function that returns the desired LLVM type.
*/
typedef struct Type {
	const char *name;

	LLVMTypeRef (*llvm_type)(void);
} Type;

LLVMTypeRef gen_llvm_str_type(void);

Type TYPE_BOOL;
Type TYPE_INT;
Type TYPE_FLOAT;
Type TYPE_RUNE;
Type TYPE_STR;

const Type **TYPES_BUILTIN;

const Type *find_type(const char *const);
_Bool add_alias(Type *const, char *const);
char *mangle_types(const Type **, unsigned, char);
