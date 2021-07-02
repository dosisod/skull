#pragma once

#include "skull/common/hashtable.h"

typedef struct Scope Scope;

/*
Stores semantic analyzer global state.
*/
typedef struct {
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
} SemanticState;

extern SemanticState SEMANTIC_STATE;

void free_state_semantic(void);
