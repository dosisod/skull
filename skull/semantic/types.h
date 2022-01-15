#pragma once

#include <stdint.h>
#include <uchar.h>

#include "skull/common/hashtable.h"
#include "skull/parse/token.h"

typedef struct Token Token;
typedef struct AstNode AstNode;

_Bool eval_bool(const Token *const);
double eval_float(const Token *const, _Bool *);
int64_t eval_integer(const Token *, _Bool *);
char32_t eval_rune(const Token *const, _Bool *);
char32_t *eval_str(const Token *const);
_Bool validate_stmt_type_alias(const AstNode *);

typedef struct Type Type;

/*
`name` is the pretty-printable name for the type.

`inner` is the inner type of a complex type, ie, `&Int`. If `NULL`, it is
assumed to be a plain type.
*/
typedef struct Type {
	union {
		const char *name;
		char *dyn_name;
	};

	const Type *inner;
} Type;

extern Type TYPE_BOOL;
extern Type TYPE_INT;
extern Type TYPE_FLOAT;
extern Type TYPE_RUNE;
extern Type TYPE_STR;
extern Type TYPE_VOID;

extern Type **TYPES_BUILTIN;

Type *find_type(const char *const);
_Bool is_reference(const Type *);
Type *get_reference_type(const Type *);
Type *token_to_type(const Token *);
