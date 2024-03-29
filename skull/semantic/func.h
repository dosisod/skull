#pragma once

#include "skull/parse/ast_node.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"

/*
Stores a function declaration.

`ref` is for storing arbitrary information, for use in the backend

`type` same as `ref`.

`num_params` stores the number of params a function can take.

`param_types` is the Skull type for each function param.

`return_type` is the Skull type that the function returns.
*/
typedef struct FunctionDeclaration {
	void *ref;
	void *type;

	unsigned short num_params;
	const Type **param_types;
	char32_t **param_names;

	const Type *return_type;

	_Bool was_called : 1;
	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam **params;
} FunctionDeclaration;

_Bool validate_stmt_func_decl(SemanticState *, const AstNode *);

void free_function_declaration(Symbol *);

Symbol *find_func_by_name(SemanticState *, char *);
