#pragma once

#include <stdint.h>
#include <uchar.h>

#include "skull/common/hashtable.h"
#include "skull/parse/token.h"

typedef struct Token Token;

_Bool eval_bool(const Token *const);
double eval_float(const Token *const, _Bool *);
int64_t eval_integer(const Token *, _Bool *);
char32_t eval_rune(const Token *const, _Bool *);
char32_t *eval_str(const Token *const);

/*
Types are simply named strings.
*/
typedef const char * Type;

extern Type TYPE_BOOL;
extern Type TYPE_INT;
extern Type TYPE_FLOAT;
extern Type TYPE_RUNE;
extern Type TYPE_STR;
extern Type TYPE_VOID;

extern Type **TYPES_BUILTIN;

Type find_type(const char *const);

void free_ht_type_alias(HashItem *);
