#include <stdbool.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/eval/types/defs.h"
#include "skull/llvm/shared.h"

#include "skull/eval/types/types.h"

/*
Returns pointer to type with name `name`.
*/
const Type __attribute__((pure)) *find_type(const char *const name) {
	const Type *const alias = ht_get(SKULL_STATE.type_aliases, name);
	if (alias) return alias;

	const Type **type = TYPES_BUILTIN;

	while (*type) {
		if (strcmp(name, (*type)->name) == 0) return *type;

		type++;
	}

	return NULL;
}

/*
Add named `alias` for `type`.

Return `true` if alias was added, `false` if it already exists.
*/
bool add_alias(Type *const type, char *const alias) {
	return ht_add(SKULL_STATE.type_aliases, alias, type);
}

/*
Return mangled type name which consists of `types`.

Use `ident` as an identifer for what the mandled type is.
*/
char *mangle_types(const Type **types, unsigned num_types, char ident) {
	// mangled type is in form "_x<>" with the types being in the <>'s
	size_t len = 4;

	for RANGE(i, num_types) { // NOLINT
		len += strlen(types[i]->name) + 1;
	}

	char *name = Malloc(len);
	name[0] = '_';
	name[1] = ident;
	name[2] = '<';

	size_t at = 3;

	for RANGE(i, num_types) { // NOLINT
		const size_t tmp_len = strlen(types[i]->name);

		strncpy(name + at, types[i]->name, tmp_len);
		at += tmp_len;
		name[at] = ',';
		at++;
	}

	name[at - 1] = '>';
	name[at] = '\0';

	return name;
}

/*
Return newly created template type with called `name` of type `llvm_type`.
*/
TemplateType *add_template_type(char *name, LLVMTypeRef *llvm_type) {
	TemplateType *new_type;
	new_type = Malloc(sizeof *new_type);

	*new_type = (TemplateType){
		.name = name,
		.llvm_type = llvm_type
	};

	ht_add(SKULL_STATE.template_types, name, new_type);

	return new_type;
}

Type TYPE_BOOL = {
	.name = "Bool",
	.llvm_type = gen_llvm_bool_type
};

Type TYPE_INT = {
	.name = "Int",
	.llvm_type = gen_llvm_int_type
};

Type TYPE_FLOAT = {
	.name = "Float",
	.llvm_type = gen_llvm_float_type
};

Type TYPE_RUNE = {
	.name = "Rune",
	.llvm_type = gen_llvm_rune_type
};

Type TYPE_STR = {
	.name = "Str",
	.llvm_type = gen_llvm_str_type
};

LLVMTypeRef gen_llvm_bool_type(void) {
	return LLVMInt1TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_int_type(void) {
	return LLVMInt64TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_float_type(void) {
	return LLVMDoubleTypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_rune_type(void) {
	return LLVMInt32TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_str_type(void) {
	return LLVMPointerType(
		LLVMInt8TypeInContext(SKULL_STATE.ctx),
		0
	);
}

const Type **TYPES_BUILTIN = (const Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	NULL
};

void free_ht_type_alias(HashItem *item) {
	free((char *)item->key);
}
