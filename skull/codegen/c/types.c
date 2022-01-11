#include <string.h>

#include "skull/codegen/c/types.h"

CType __attribute__((pure)) skull_type_to_c_type(const Type *type) {
	if (type == &TYPE_INT) return TYPE_INT_C;
	if (type == &TYPE_FLOAT) return TYPE_FLOAT_C;
	if (type == &TYPE_RUNE) return TYPE_RUNE_C;
	if (type == &TYPE_STR) return TYPE_STR_C;
	if (type == &TYPE_BOOL) return TYPE_BOOL_C;
	if (type == &TYPE_VOID) return TYPE_VOID_C;
	if (type->inner) {
		// TODO(dosisod): name is statically allocated, but should be on heap
		CType inner = skull_type_to_c_type(type->inner);

		const size_t len = strlen(inner);
		static char name[128];
		strncpy(name, inner, len);
		name[len] = '*';
		name[len + 1] = '\0';

		return name;
	}

	return NULL;
}
