#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"

/*
Shared state for a given Skull program.
*/
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMValueRef current_func;
	LLVMValueRef main_func;
	Scope *scope;
} SkullState;

SkullState SKULL_STATE;
