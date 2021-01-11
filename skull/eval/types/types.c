#include <stdbool.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/eval/types/defs.h"

#include "skull/eval/types/types.h"

/*
Returns pointer to type with name `name`.
*/
const Type __attribute__((pure)) *find_type(const char *const name) {
	const Type **type = TYPES_AVAILABLE;

	while (*type) {
		if (strcmp(name, (*type)->name) == 0) {
			return *type;
		}

		for RANGE(i, (*type)->num_aliases) {
			if (strcmp(name, (*type)->aliases[i]) == 0) {
				return *type;
			}
		}
		type++;
	}

	return NULL;
}

/*
Add named `alias` for `type`.

Return `true` if alias was added, `false` if it already exists.
*/
bool add_alias(Type *const type, char *const alias) {
	if (find_type(alias)) {
		return false;
	}

	if (!type->num_aliases) {
		type->aliases = Malloc(sizeof(char *));
	}

	type->aliases[type->num_aliases] = alias;

	type->num_aliases++;

	return true;
}

Type TYPE_BOOL = {
	.name = "bool",
	.llvm_type = &LLVMInt1Type
};

Type TYPE_INT = {
	.name = "int",
	.llvm_type = &LLVMInt64Type
};

Type TYPE_FLOAT = {
	.name = "float",
	.llvm_type = &LLVMDoubleType
};

Type TYPE_RUNE = {
	.name = "rune",
	.llvm_type = &LLVMInt32Type
};

Type TYPE_STR = {
	.name = "str",
	.llvm_type = &gen_llvm_str_type
};

LLVMTypeRef gen_llvm_str_type(void) {
	return LLVMPointerType(LLVMInt8Type(), 0);
}

const Type **TYPES_AVAILABLE = (const Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	NULL
};
