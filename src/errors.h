#pragma once

#include <stdbool.h>
#include <uchar.h>

#define ERROR_INVALID_INPUT 0
#define ERROR_VAR_ALREADY_DEFINED 1
#define ERROR_CANNOT_ASSIGN_CONST 2
#define ERROR_TYPE_MISMATCH 3
#define ERROR_OVERFLOW 4
#define ERROR_MISSING_ASSIGNMENT 5
#define ERROR_VAR_NOT_FOUND 6

extern const char32_t *ERROR_MSG[];
const char32_t **ERROR_MSG_LAST;

/*
Returns if a given string is an error msg.
*/
bool is_error_msg(const char32_t *str);