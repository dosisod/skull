#pragma once

#include "../parse/types.h"

typedef struct variable_t {
	type_t *type;
	const char32_t *name;
	bool is_const;

	size_t bytes;
	uint8_t *mem;
} variable_t;

variable_t *make_variable(const char32_t *, const char32_t *, bool);

const char32_t *variable_write(const variable_t *, const void *);
void variable_read(void *, const variable_t *);

void free_variable(variable_t *);

char32_t *fmt_var(const variable_t *);