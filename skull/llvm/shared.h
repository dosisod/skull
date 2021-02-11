#pragma once

#include <llvm-c/Core.h>

#include "skull/common/hashtable.h"
#include "skull/eval/scope.h"

/*
Shared state for a given Skull program.
*/
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	const char *filename;
	LLVMModuleRef module;
	LLVMValueRef current_func;
	LLVMValueRef main_func;
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
	HashTable *template_types;
} SkullState;

SkullState SKULL_STATE;

void free_state(SkullState);
