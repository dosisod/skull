#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/hashtable.h"

typedef struct Scope Scope;

/*
Shared state for a given Skull program.
*/
typedef struct {
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
} SkullState;

extern SkullState SKULL_STATE;

void free_state(void);
