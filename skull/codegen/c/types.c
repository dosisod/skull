#include "skull/codegen/c/types.h"

CType __attribute__((pure)) skull_type_to_c_type(Type *type) {
	if (type == &TYPE_INT) return TYPE_INT_C;
	if (type == &TYPE_FLOAT) return TYPE_FLOAT_C;
	if (type == &TYPE_RUNE) return TYPE_RUNE_C;
	if (type == &TYPE_STR) return TYPE_STR_C;
	if (type == &TYPE_BOOL) return TYPE_BOOL_C;
	if (type == &TYPE_VOID) return TYPE_VOID_C;
	if (type == &TYPE_INT_REF) return TYPE_INT_REF_C;
	if (type == &TYPE_FLOAT_REF) return TYPE_FLOAT_REF_C;
	if (type == &TYPE_RUNE_REF) return TYPE_RUNE_REF_C;
	if (type == &TYPE_STR_REF) return TYPE_STR_REF_C;
	if (type == &TYPE_BOOL_REF) return TYPE_BOOL_REF_C;

	return NULL;
}
