#pragma once

#include "skull/common/hashtable.h"

typedef struct Scope Scope;
typedef struct FunctionDeclaration FunctionDeclaration;

/*
Stores semantic analyzer global state.
*/
typedef struct {
	Scope *scope;
	FunctionDeclaration *main_func;
	FunctionDeclaration *current_func;
	FunctionDeclaration *last_func;
} SemanticState;

extern SemanticState SEMANTIC_STATE;

void setup_semantic_state(void);
void free_semantic_state(void);
