#include <string.h>

#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"

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
	.bytes = sizeof(_Bool),
	.to_string = &fmt_bool_type
};

const Type TYPE_INT = {
	.name = "int",
	.bytes = sizeof(SkullInt),
	.to_string = &fmt_int_type
};

const Type TYPE_FLOAT = {
	.name = "float",
	.bytes = sizeof(SkullFloat),
	.to_string = &fmt_float_type
};

const Type TYPE_RUNE = {
	.name = "rune",
	.bytes = sizeof(SkullRune),
	.to_string = &fmt_rune_type
};

const Type TYPE_STR = {
	.name = "str",
	.bytes = sizeof(SkullStr),
	.to_string = &fmt_str_type
};

const Type **TYPES_AVAILABLE = (const Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	NULL
};
