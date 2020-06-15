#pragma once

#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../errors.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"
#include "../parse/types.h"

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

	if (found_type==NULL || !is_valid_identifier_str(name)) {
		return NULL;
	}

	variable_t *var=malloc(sizeof(variable_t));
	DIE_IF_MALLOC_FAILS(var);

	const size_t len=wcslen(name);
	wchar_t *name_copy=malloc((len + 1) * sizeof(wchar_t));
	DIE_IF_MALLOC_FAILS(name_copy);

	wcslcpy(name_copy, name, len + 1);

	var->name=name_copy;
	var->type=found_type;
	var->is_const=is_const;
	var->bytes=found_type->bytes;

	uint8_t *mem=calloc(found_type->bytes, sizeof(uint8_t));
	DIE_IF_MALLOC_FAILS(mem);

	var->mem=mem;
	return var;
}

/*
Write `data` to `var`.

If `var` is constant, return error msg, else `NULL`.
*/
const wchar_t *variable_write(const variable_t *var, const void *data) {
	if (var->is_const) {
		return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST];
	}

	memcpy(var->mem, data, var->bytes);
	return NULL;
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
	if (var->type==find_type(L"bool")) {
		bool data=false;
		variable_read(&data, var);

		wchar_t *ret=wcsdup(data ? L"true" : L"false");
		DIE_IF_MALLOC_FAILS(ret);

		return ret;
	}
	if (var->type==find_type(L"str")) {
		const wchar_t *data=NULL;
		variable_read(&data, var);

		size_t len=wcslen(data);

		//add 3 for ""s and NULL terminator
		wchar_t *ret=malloc(sizeof(wchar_t) * (len + 3));
		DIE_IF_MALLOC_FAILS(ret);

		wcsncpy(ret + 1, data, len);
		ret[0]=L'\"';
		ret[len + 1]=L'\"';
		ret[len + 2]=L'\0';

		return ret;
	}
	if (var->type==find_type(L"char")) {
		wchar_t *ret=malloc(sizeof(wchar_t) * 4);
		DIE_IF_MALLOC_FAILS(ret);

		ret[0]=L'\'';
		ret[1]=*var->mem;
		ret[2]=L'\'';
		ret[3]=L'\0';

		return ret;
	}

	char *tmp=NULL;
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
		DIE_IF_MALLOC_FAILS(tmp);

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
		DIE_IF_MALLOC_FAILS(tmp);

		wrote=snprintf(tmp, (unsigned long int)needed, "%Lg", data);
	}

	if (wrote<0) {
		free(tmp);
		return NULL;
	}

	wchar_t *ret=malloc(sizeof(wchar_t) * (unsigned long int)needed);
	DIE_IF_MALLOC_FAILS(ret);

	mbstowcs(ret, tmp, (unsigned long int)needed);

	free(tmp);
	return ret;
}