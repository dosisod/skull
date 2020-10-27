#pragma once

#include <llvm-c/Core.h>

#include "skull/parse/ast/node.h"

typedef struct ExternalFunction ExternalFunction;

typedef struct ExternalFunction {
	char *name;
	LLVMValueRef function;
	LLVMTypeRef type;
	unsigned num_params;

	ExternalFunction *next;
} ExternalFunction;

ExternalFunction *external_functions;

void declare_external_function(AstNode *);
void llvm_make_var_def(AstNode **);
