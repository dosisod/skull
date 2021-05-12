#include <stdbool.h>
#include <string.h>

#include "skull/codegen/shared.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/compiler/types/types.h"

/*
Returns pointer to type with name `name`.
*/
Type __attribute__((pure)) find_type(const char *const name) {
	Type alias = ht_get(SKULL_STATE.type_aliases, name);
	if (alias) return alias;

	Type **type = TYPES_BUILTIN;

	while (*type) {
		if (strcmp(name, **type) == 0) return **type;

		type++;
	}

	return NULL;
}

/*
Add named `alias` for `type`.

Return `true` if alias was added, `false` if it already exists.
*/
bool add_alias(Type type, char *const alias) {
	if (!SKULL_STATE.type_aliases) {
		SKULL_STATE.type_aliases = ht_create();
	}

	return ht_add(SKULL_STATE.type_aliases, alias, (void *)type);
}

/*
Return mangled type name which consists of `types`.

Use `ident` as an identifer for what the mandled type is.
*/
char *mangle_types(Type *types, unsigned num_types, char ident) {
	// mangled type is in form "_x<>" with the types being in the <>'s
	size_t len = 4;

	for RANGE(i, num_types) { // NOLINT
		len += strlen(types[i]) + 1;
	}

	char *name = Malloc(len);
	name[0] = '_';
	name[1] = ident;
	name[2] = '<';

	size_t at = 3;

	for RANGE(i, num_types) { // NOLINT
		const size_t tmp_len = strlen(types[i]);
		memcpy(name + at, types[i], tmp_len);

		at += tmp_len;
		name[at] = ',';
		at++;
	}

	name[at - 1] = '>';
	name[at] = '\0';

	return name;
}

Type TYPE_BOOL = "Bool";
Type TYPE_INT = "Int";
Type TYPE_FLOAT = "Float";
Type TYPE_RUNE = "Rune";
Type TYPE_STR = "Str";

Type **TYPES_BUILTIN = (Type *[]){
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
