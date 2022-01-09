#pragma once

#include "skull/semantic/types.h"

typedef const char * CType;

#define TYPE_INT_C "int64_t"
#define TYPE_FLOAT_C "double"
#define TYPE_RUNE_C "char32_t"
#define TYPE_STR_C "char*"
#define TYPE_BOOL_C "bool"
#define TYPE_VOID_C "void"
#define TYPE_INT_REF_C TYPE_INT_C"*"
#define TYPE_FLOAT_REF_C TYPE_FLOAT_C"*"
#define TYPE_RUNE_REF_C TYPE_RUNE_C"*"
#define TYPE_STR_REF_C TYPE_STR_C"*"
#define TYPE_BOOL_REF_C TYPE_BOOL_C"*"

CType skull_type_to_c_type(Type *);
