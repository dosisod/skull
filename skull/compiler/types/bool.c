#include <stdbool.h>

#include "skull/compiler/types/bool.h"

/*
Return bool converted from `token`
*/
bool eval_bool(const Token *const token) {
	// since token is either "true" or "false", just check length of token
	return token_len(token) == 4;
}
