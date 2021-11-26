#pragma once

typedef struct {
	char *tree;
	char *globals;
	_Bool called_int_pow;
	_Bool called_float_pow;
	_Bool called_strcmp;
	unsigned indent_lvl;
} SkullStateC;

extern SkullStateC SKULL_STATE_C;

char *get_indentation(void);
