#pragma once

#include "skull/common/hashtable.h"

typedef struct Scope Scope;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct Symbol Symbol;

/*
Stores semantic analyzer global state.
*/
typedef struct {
	Scope *scope;
	Symbol *main_func;
	Symbol *current_func;
	Symbol *last_func;
	unsigned while_loop_depth;
	HashTable *dynamic_refs;
} SemanticState;

SemanticState *setup_semantic_state(void);
void free_semantic_state(SemanticState *);
