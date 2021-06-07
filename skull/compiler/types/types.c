#include <stdbool.h>
#include <string.h>

#include "skull/codegen/shared.h"
#include "skull/common/malloc.h"

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
bool state_add_alias(Type type, char *const alias) {
	if (!SKULL_STATE.type_aliases) {
		SKULL_STATE.type_aliases = ht_create();
	}

	return ht_add(SKULL_STATE.type_aliases, alias, (void *)type);
}

Type TYPE_BOOL = "Bool";
Type TYPE_INT = "Int";
Type TYPE_FLOAT = "Float";
Type TYPE_RUNE = "Rune";
Type TYPE_STR = "Str";
Type TYPE_VOID = "Void";

Type **TYPES_BUILTIN = (Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	&TYPE_VOID,
	NULL
};

void free_ht_type_alias(HashItem *item) {
	free((char *)item->key);
}
