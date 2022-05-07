#pragma once

#include "skull/codegen/llvm/fwd_decl.h"

typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct Symbol Symbol;

typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	Symbol *current_func;
	Symbol *main_func;
	LLVMBasicBlockRef current_while_cond;
	LLVMBasicBlockRef current_while_end;
	LLVMTargetMachineRef target_machine;
} SkullStateLLVM;

SkullStateLLVM *setup_llvm_state(void);
void free_llvm_state(SkullStateLLVM *);
