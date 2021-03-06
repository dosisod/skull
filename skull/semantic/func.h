#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/parse/ast_node.h"

typedef const char * Type;
typedef struct HashItem HashItem;

/*
Stores a function declaration.

`name` is the name of the declared function.

`location` is the location of the function name.

`ref` is the LLVM function handle.

`type` is the LLVM function type.

`num_params` stores the number of params a function can take.

`param_types` is the Skull type for each function param.

`return_type` is the Skull type that the function returns.
*/
typedef struct FunctionDeclaration {
	char *name;
	Location location;

	LLVMValueRef ref;
	LLVMTypeRef type;

	unsigned short num_params;
	Type *param_types;
	char32_t **param_names;

	Type return_type;

	_Bool was_called : 1;
	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam **params;
} FunctionDeclaration;

_Bool validate_stmt_func_decl(const AstNode *);

void free_function_declaration(HashItem *);

FunctionDeclaration *find_func_by_name(const char *);
