#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"
#include "skull/parse/ast/node.h"

typedef struct ExternalFunction ExternalFunction;

/*
Stores an external function declaration.

`name` is the name of the declared function.

`function` is the LLVM function handle.

`type` is the LLVM function type.

`num_params` stores the number of params a function can take.

`param_types` is the Skull type for each function param.

`return_type` is the Skull type that the function returns.

`next` stores the next external function declaration.
*/
typedef struct ExternalFunction {
	char *name;
	LLVMValueRef function;
	LLVMTypeRef type;

	unsigned num_params;
	const Type *param_types;
	const char32_t *param_names;

	const Type *return_type;

	ExternalFunction *next;
} ExternalFunction;

ExternalFunction *EXTERNAL_FUNCTIONS;

void declare_external_function(AstNode *);

LLVMValueRef llvm_make_function(const AstNode *const);

void define_function(const AstNode *const);
