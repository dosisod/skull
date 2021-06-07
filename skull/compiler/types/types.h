#pragma once

#include "skull/common/hashtable.h"

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
_Bool state_add_alias(Type, char *const);

void free_ht_type_alias(HashItem *);
