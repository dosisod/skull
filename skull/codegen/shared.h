#pragma once

#include "skull/codegen/func.h"
#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/hashtable.h"
#include "skull/compiler/scope.h"

/*
Shared state for a given Skull program.
*/
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	const char *filename;
	LLVMModuleRef module;
	FunctionDeclaration *current_func;
	FunctionDeclaration *main_func;
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
	HashTable *template_types;
} SkullState;

extern SkullState SKULL_STATE;

void free_state(SkullState *);

void setup_state(SkullState *, const char *);
