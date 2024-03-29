#pragma once

#include <stdint.h>
#include <uchar.h>

#include "skull/common/hashtable.h"
#include "skull/parse/token.h"
#include "skull/semantic/shared.h"

typedef struct Token Token;
typedef struct AstNode AstNode;

_Bool eval_bool(const Token *const);
double eval_float(const Token *const, _Bool *);
int64_t eval_integer(const Token *, _Bool *);
char32_t eval_rune(const Token *const, _Bool *);
char32_t *eval_str(const Token *const);
_Bool validate_stmt_type_alias(SemanticState *, const AstNode *);

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

const Type *find_type(SemanticState *, char *const);
const Type *find_builtin_type(const char *const);
_Bool is_reference(const Type *);
Type *get_reference_type(SemanticState *, const Type *);
const Type *token_to_type(SemanticState *, const Token *);
_Bool is_identifier_like(const Token *);
