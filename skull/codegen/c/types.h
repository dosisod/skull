#pragma once

#include "skull/semantic/types.h"

typedef const char * CType;

#define TYPE_INT_C "int64_t"
#define TYPE_FLOAT_C "double"
#define TYPE_RUNE_C "char32_t"
#define TYPE_STR_C "char32_t *"
#define TYPE_BOOL_C "bool"
#define TYPE_VOID_C "void"

CType skull_type_to_c_type(Type);
