#pragma once

#include "skull/semantic/shared.h"

typedef struct {
	char *tree;
	char *globals;
	_Bool called_int_pow;
	_Bool called_float_pow;
	_Bool called_strcmp;
	unsigned indent_lvl;
} SkullStateC;

SkullStateC *setup_c_state(SemanticState *);
char *get_indentation(const SkullStateC *);
void free_c_state(SkullStateC *);
