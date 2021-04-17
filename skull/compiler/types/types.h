#pragma once

#include <stddef.h>

#include <llvm-c/Core.h>

#include "skull/common/hashtable.h"

// Types are simply named strings.
typedef const char * Type;

/*
Similar to `Type` except that LLVM type is generated at runtime.
*/
typedef struct TemplateType {
	char *name;

	LLVMTypeRef *llvm_type;
} TemplateType;

Type TYPE_BOOL;
Type TYPE_INT;
Type TYPE_FLOAT;
Type TYPE_RUNE;
Type TYPE_STR;

Type **TYPES_BUILTIN;

Type find_type(const char *const);
_Bool add_alias(Type, char *const);
char *mangle_types(Type *, unsigned, char);

void free_ht_type_alias(HashItem *);
