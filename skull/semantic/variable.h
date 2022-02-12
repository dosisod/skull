#pragma once

#include <uchar.h>

#include "skull/parse/token.h"
#include "skull/semantic/types.h"

typedef struct AstNodeExpr AstNodeExpr;

/*
`type` is the Skull type of the variable.

`name` is the name of the variable.

`is_const` stores whether the variable is const or not.

`is_const_lit` stores whether the variable is const literal.

`is_global` stores whether the variable was declared at the top level of a
module.

`was_reassigned` stores whether a (mutable) variable was reassigned.

`was_read` stores whether a variable was read from or not.

`implicitly_typed` is true when variable type is auto-deduced.

`is_exported` is true when the variable is marked as exported.

`is_defined` is set once a variable is done being assigned (used to detect
self-referencing the variable currently being assignment to).

`location` stores the location where the variable was first defined.

`ref` stores arbitrary data, for use in the backend.
*/
typedef struct Variable {
	const Type *type;
	char *name;
	char *linkage_name;
	_Bool is_const : 1;
	_Bool is_const_lit : 1;
	_Bool is_global : 1;
	_Bool was_reassigned : 1;
	_Bool was_read : 1;
	_Bool implicitly_typed : 1;
	_Bool is_exported : 1;
	_Bool is_defined : 1;

	Location location;

	void *ref;
	AstNodeExpr *expr;
} Variable;

Variable *make_variable(const Type *, const char32_t *const, _Bool);

void free_variable(Variable *);

void variable_disable_warnings(Variable *);
