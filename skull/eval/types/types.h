#pragma once

#include <stddef.h>

#include <llvm-c/Core.h>

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

const Type TYPE_BOOL;
const Type TYPE_INT;
const Type TYPE_FLOAT;
const Type TYPE_RUNE;
const Type TYPE_STR;

const Type **TYPES_AVAILABLE;

const Type *find_type(const char *const);
