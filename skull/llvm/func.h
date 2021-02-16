#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/types/types.h"
#include "skull/llvm/var.h"
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

	unsigned short num_params;
	const Type **param_types;
	char32_t **param_names;

	const Type *return_type;

	FunctionDeclaration *next;
} FunctionDeclaration;

void gen_stmt_func_decl(const AstNode *const);

Expr gen_expr_function_call(const AstNode *const, const Type *const);

void define_function(const AstNode *const, FunctionDeclaration *);

FunctionDeclaration *find_function(const char *);

void free_function_declaration(FunctionDeclaration *);
