#pragma once

#include <stdbool.h>

#include <llvm-c/Core.h>

#include "skull/codegen/var.h"
#include "skull/compiler/types/types.h"
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
	Type *param_types;
	char32_t **param_names;

	Type return_type;
} FunctionDeclaration;

bool gen_stmt_func_decl(const AstNode *const);

Expr gen_expr_function_call(const AstNodeExpr *const, Type, bool *);

void free_function_declaration(HashItem *);

FunctionDeclaration *find_func_by_name(const char *);
