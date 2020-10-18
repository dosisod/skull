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
	.bytes = sizeof(_Bool)
};

const Type TYPE_INT = {
	.name = "int",
	.bytes = sizeof(SkullInt)
};

const Type TYPE_FLOAT = {
	.name = "float",
	.bytes = sizeof(SkullFloat)
};

const Type TYPE_RUNE = {
	.name = "rune",
	.bytes = sizeof(SkullRune)
};

const Type TYPE_STR = {
	.name = "str",
	.bytes = sizeof(SkullStr)
};

const Type **TYPES_AVAILABLE = (const Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	NULL
};
