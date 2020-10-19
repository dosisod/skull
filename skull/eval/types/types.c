#include <string.h>

#include "skull/eval/types/defs.h"

#include "skull/eval/types/types.h"

/*
Returns pointer to type with name `name`.
*/
const Type __attribute__((pure)) *find_type(const char *const name) {
	const Type **head = TYPES_AVAILABLE;

	while (*head) {
		if (strcmp(name, (*head)->name) == 0) {
			return *head;
		}
		head++;
	}

	return NULL;
}

const Type TYPE_BOOL = {
	.name = "bool",
	.llvm_type = &LLVMInt1Type
};

const Type TYPE_INT = {
	.name = "int",
	.llvm_type = &LLVMInt64Type
};

const Type TYPE_FLOAT = {
	.name = "float",
	.llvm_type = &LLVMDoubleType
};

const Type TYPE_RUNE = {
	.name = "rune",
	.llvm_type = &LLVMInt32Type
};

const Type TYPE_STR = {
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
