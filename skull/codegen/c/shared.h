#pragma once

typedef struct {
	char *tree;
	char *globals;
	_Bool called_int_pow;
	_Bool called_float_pow;
	_Bool called_strcmp;
	unsigned indent_lvl;
} SkullStateC;

SkullStateC *setup_c_state(void);
char *get_indentation(const SkullStateC *);
void free_c_state(void);
