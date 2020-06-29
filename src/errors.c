#include <stddef.h>

#include "errors.h"

const char32_t *ERR_INVALID_INPUT=U"invalid input";
const char32_t *ERR_VAR_ALREADY_DEFINED=U"variable already defined";
const char32_t *ERR_CANNOT_ASSIGN_CONST=U"cannot assign to const";
const char32_t *ERR_TYPE_MISMATCH=U"error, type mismatch";
const char32_t *ERR_OVERFLOW=U"overflow occurred";
const char32_t *ERR_MISSING_ASSIGNMENT=U"missing value in variable assignment";
const char32_t *ERR_VAR_NOT_FOUND=U"variable not found";

__attribute__((pure)) bool is_error_msg(const char32_t *str) {
	return (
		(str==ERR_INVALID_INPUT) ||
		(str==ERR_VAR_ALREADY_DEFINED) ||
		(str==ERR_CANNOT_ASSIGN_CONST) ||
		(str==ERR_TYPE_MISMATCH) ||
		(str==ERR_OVERFLOW) ||
		(str==ERR_MISSING_ASSIGNMENT) ||
		(str==ERR_VAR_NOT_FOUND)
	);
}