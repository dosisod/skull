#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"
#include "skull/parse/ast_node.h"

typedef struct FunctionDeclaration FunctionDeclaration;

/*
Stores a function declaration.

`name` is the name of the declared function.

`function` is the LLVM function handle.

`type` is the LLVM function type.

`num_params` stores the number of params a function can take.

`param_types` is the Skull type for each function param.

`return_type` is the Skull type that the function returns.

`next` stores the next function declaration.
*/
typedef struct FunctionDeclaration {
	char *name;
	LLVMValueRef function;
	LLVMTypeRef type;

	unsigned num_params;
	const Type *param_types;
	const char32_t *param_names;

	const Type *return_type;

	FunctionDeclaration *next;
} FunctionDeclaration;

FunctionDeclaration *FUNCTION_DECLARATIONS;

void declare_function(AstNode *);

LLVMValueRef llvm_make_function_call(const AstNode *const);

void define_function(const AstNode *const, FunctionDeclaration *);
