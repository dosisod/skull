#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../parse/classify.h"
#include "../parse/tokenize.h"
#include "../parse/types.h"

#define VARIABLE_WRITE_OK 0
#define VARIABLE_WRITE_ECONST 1

typedef struct variable_t {
	type_t *type;
	const wchar_t *name;
	bool is_const;

	size_t bytes;
	uint8_t *mem;
} variable_t;

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const wchar_t *type, const wchar_t *name, bool is_const) {
	type_t *found_type=find_type(type);

	if (found_type==NULL || !is_valid_identifier(name)) {
		return NULL;
	}

	variable_t *var=malloc(sizeof(variable_t));

	const size_t len=wcslen(name);
	wchar_t *name_copy=malloc((len + 1) * sizeof(wchar_t));
	wcslcpy(name_copy, name, len + 1);

	var->name=name_copy;
	var->type=found_type;
	var->is_const=is_const;
	var->bytes=found_type->bytes;

	uint8_t *mem=calloc(found_type->bytes, sizeof(uint8_t));
	var->mem=mem;

	return var;
}

/*
Write `data` to `var`.

If `var` is const, `VARIABLE_WRITE_ECONST` is returned.
Upon success, `VARIABLE_WRITE_OK` is returned.
*/
uint8_t variable_write(const variable_t *var, const void *data) {
	if (var->is_const) {
		return VARIABLE_WRITE_ECONST;
	}

	memcpy(var->mem, data, var->bytes);
	return VARIABLE_WRITE_OK;
}

/*
Read variable `var`s memory into `dest`.
*/
void variable_read(void *dest, const variable_t *var) {
	memcpy(dest, var->mem, var->bytes);
}

/*
Free a given `variable_t` variable.
*/
void free_variable(variable_t *var) {
	if (var!=NULL) {
		free((wchar_t*)var->name);
		free(var->mem);
		free(var);
	}
}

/*
Return string representation of a variable.

The result of this function must be freed.
*/
wchar_t *fmt_var(const variable_t *var) {
	char *tmp=NULL;
	wchar_t *ret=NULL;

	int needed=-1;
	int wrote=-1;
	if (var->type==find_type(L"int")) {
		int64_t data=0;
		variable_read(&data, var);

		needed=snprintf(NULL, 0, "%li", data) + 1;
		if (needed<0) {
			return NULL;
		}
		tmp=malloc(sizeof(char) * (unsigned long int)needed);
		wrote=snprintf(tmp, (unsigned long int)needed, "%li", data);
	}
	else if (var->type==find_type(L"float")) {
		long double data=0;
		variable_read(&data, var);

		needed=snprintf(NULL, 0, "%Lg", data) + 1;
		if (needed<0) {
			return NULL;
		}
		tmp=malloc(sizeof(char) * (unsigned long int)needed);
		wrote=snprintf(tmp, (unsigned long int)needed, "%Lg", data);
	}

	if (wrote<0) {
		free(tmp);
		return NULL;
	}

	ret=malloc(sizeof(wchar_t) * (unsigned long int)needed);
	mbstowcs(ret, tmp, (unsigned long int)needed);

	free(tmp);
	return ret;
}