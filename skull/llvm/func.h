#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"
#include "skull/parse/ast/node.h"

typedef struct ExternalFunction ExternalFunction;

typedef struct ExternalFunction {
	char *name;
	LLVMValueRef function;
	LLVMTypeRef type;

	unsigned num_params;
	const Type *param_types;

	ExternalFunction *next;
} ExternalFunction;

ExternalFunction *EXTERNAL_FUNCTIONS;

void declare_external_function(AstNode *);
void llvm_make_var_def(AstNode **);
