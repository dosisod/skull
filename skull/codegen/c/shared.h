#pragma once

typedef struct {
	char *tree;
	_Bool called_int_pow;
	_Bool called_float_pow;
	_Bool called_strcmp;
} SkullStateC;

extern SkullStateC SKULL_STATE_C;
