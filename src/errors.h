#pragma once

#include <stdbool.h>
#include <uchar.h>

const char32_t *ERR_INVALID_INPUT;
const char32_t *ERR_VAR_ALREADY_DEFINED;
const char32_t *ERR_CANNOT_ASSIGN_CONST;
const char32_t *ERR_TYPE_MISMATCH;
const char32_t *ERR_OVERFLOW;
const char32_t *ERR_MISSING_ASSIGNMENT;
const char32_t *ERR_VAR_NOT_FOUND;

/*
Returns if a given string is an error msg.
*/
bool is_error_msg(const char32_t *str);