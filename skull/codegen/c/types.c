#include <string.h>

#include "skull/common/malloc.h"

#include "skull/codegen/c/types.h"

char *__attribute__((pure)) skull_type_to_c_type(const Type *type) {
	if (type == &TYPE_INT) return strdup(TYPE_INT_C);
	if (type == &TYPE_FLOAT) return strdup(TYPE_FLOAT_C);
	if (type == &TYPE_RUNE) return strdup(TYPE_RUNE_C);
	if (type == &TYPE_STR) return strdup(TYPE_STR_C);
	if (type == &TYPE_BOOL) return strdup(TYPE_BOOL_C);
	if (type == &TYPE_VOID) return strdup(TYPE_VOID_C);
	if (type->inner) {
		char *inner = skull_type_to_c_type(type->inner);

		const size_t len = strlen(inner);

		char *type_name = Realloc(inner, len + 2);
		type_name[len] = '*';
		type_name[len + 1] = '\0';

		return type_name;
	}

	return NULL;
}
