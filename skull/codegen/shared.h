#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/hashtable.h"

typedef struct Scope Scope;
typedef struct FunctionDeclaration FunctionDeclaration;

/*
Shared state for a given Skull program.
*/
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	FunctionDeclaration *current_func;
	FunctionDeclaration *main_func;
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
	HashTable *template_types;
} SkullState;

extern SkullState SKULL_STATE;

void free_state(void);

void setup_state(void);
