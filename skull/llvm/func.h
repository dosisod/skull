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

	const Type *return_type;

	ExternalFunction *next;
} ExternalFunction;

ExternalFunction *EXTERNAL_FUNCTIONS;

void declare_external_function(AstNode *);

LLVMValueRef llvm_make_function(const AstNode *const);
