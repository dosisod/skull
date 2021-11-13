#include "skull/codegen/c/types.h"

const char *skull_type_to_c_type(Type type) {
	if (type == TYPE_INT) return "int64_t";
	if (type == TYPE_FLOAT) return "double";
	if (type == TYPE_RUNE) return "char32_t";
	if (type == TYPE_STR) return "char32_t *";
	if (type == TYPE_BOOL) return "bool";
	if (type == TYPE_VOID) return "void";

	return NULL;
}