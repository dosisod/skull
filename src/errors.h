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
const char32_t *ERR_NON_INT_RETURN;
const char32_t *ERR_CANNOT_ADD;
const char32_t *ERR_ADD_UNAVAILABLE;
const char32_t *ERR_TYPE_TYPE_BAD;
const char32_t *ERR_BOOL_BAD;
const char32_t *ERR_COMPARISON_BAD;
const char32_t *ERR_MISSING_TOKEN;
const char32_t *ERR_UNEXPECTED_PARAM;
const char32_t *ERR_INVALID_PARAMS;

bool is_error_msg(const char32_t *);