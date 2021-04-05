#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/compiler/types/bool.h"

/*
Return bool converted from `token`
*/
bool eval_bool(const Token *const token) {
	return token_cmp(U"true", token);
}
