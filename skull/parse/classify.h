#pragma once

#include <uchar.h>

#include "skull/parse/token.h"

_Bool is_type_str(const char32_t *const);
_Bool is_reserved_str(const char32_t *const);
_Bool is_constant_integer_str(const char32_t *const);
_Bool is_constant_float_str(const char32_t *const);
_Bool is_constant_bool_str(const char32_t *const);
_Bool is_constant_rune_str(const char32_t *const);
_Bool is_constant_str_str(const char32_t *const);
_Bool is_valid_identifier_str(const char32_t *const);

void classify_token(Token *const);
void classify_tokens(Token *);
