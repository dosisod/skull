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

`inner` is the inner type of a generic, ie, `Pointer<Int>`. If not specified,
it is assumed to be a plain type.
*/
typedef struct Type {
	const char *name;

	Type *inner;
} Type;

extern Type TYPE_BOOL;
extern Type TYPE_INT;
extern Type TYPE_FLOAT;
extern Type TYPE_RUNE;
extern Type TYPE_STR;
extern Type TYPE_VOID;
extern Type TYPE_BOOL_REF;
extern Type TYPE_INT_REF;
extern Type TYPE_FLOAT_REF;
extern Type TYPE_RUNE_REF;
extern Type TYPE_STR_REF;

extern Type **TYPES_BUILTIN;

Type *find_type(const char *const);
_Bool is_reference(const Type *);
