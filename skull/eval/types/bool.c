#include <stdbool.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"

#include "skull/eval/types/bool.h"

/*
Return bool converted from `token`
*/
bool eval_bool(const Token *const token) {
	return token_cmp(U"true", token);
}
