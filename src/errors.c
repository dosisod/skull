#include <stddef.h>

#include "errors.h"

const char32_t *ERR_INVALID_INPUT=U"invalid input";
const char32_t *ERR_VAR_ALREADY_DEFINED=U"variable already defined";
const char32_t *ERR_CANNOT_ASSIGN_CONST=U"cannot assign to const";
const char32_t *ERR_TYPE_MISMATCH=U"error, type mismatch";
const char32_t *ERR_OVERFLOW=U"overflow occurred";
const char32_t *ERR_MISSING_ASSIGNMENT=U"missing value in variable assignment";
const char32_t *ERR_VAR_NOT_FOUND=U"variable not found";
const char32_t *ERR_NON_INT_RETURN=U"returning non-int value from main";
const char32_t *ERR_CANNOT_ADD=U"cannot add when types differ";
const char32_t *ERR_ADD_UNAVAILABLE=U"type does not allow for addition";
const char32_t *ERR_TYPE_TYPE_BAD=U"\"type\" cannot be assigned to type";
const char32_t *ERR_BOOL_BAD=U"only \"true\" and \"false\" can be assigned to bool types";
const char32_t *ERR_COMPARISON_BAD=U"requested comparison not understood";
const char32_t *ERR_MISSING_TOKEN=U"missing expected token";
const char32_t *ERR_UNEXPECTED_PARAM=U"function does not take any parameters";
const char32_t *ERR_INVALID_PARAMS=U"invalid number of parameters given";

/*
Returns true if `str` is an error msg.
*/
__attribute__((pure)) bool is_error_msg(const char32_t *str) {
	return (
		(str==ERR_INVALID_INPUT) ||
		(str==ERR_VAR_ALREADY_DEFINED) ||
		(str==ERR_CANNOT_ASSIGN_CONST) ||
		(str==ERR_TYPE_MISMATCH) ||
		(str==ERR_OVERFLOW) ||
		(str==ERR_MISSING_ASSIGNMENT) ||
		(str==ERR_VAR_NOT_FOUND) ||
		(str==ERR_NON_INT_RETURN) ||
		(str==ERR_CANNOT_ADD) ||
		(str==ERR_ADD_UNAVAILABLE) ||
		(str==ERR_TYPE_TYPE_BAD) ||
		(str==ERR_BOOL_BAD) ||
		(str==ERR_COMPARISON_BAD) ||
		(str==ERR_MISSING_TOKEN) ||
		(str==ERR_UNEXPECTED_PARAM) ||
		(str==ERR_INVALID_PARAMS)
	);
}