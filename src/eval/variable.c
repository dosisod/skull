#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "../errors.h"
#include "../parse/classify.h"

#include "variable.h"

/*
Return string representation of the variable `var`.

The result of this function must be freed.
*/
char32_t *fmt_var(const variable_t *var) {
	if (var->type==&TYPE_BOOL) {
		bool data=false;
		variable_read(&data, var);

		char32_t *ret=c32sdup(data ? U"true" : U"false");
		DIE_IF_MALLOC_FAILS(ret);

		return ret;
	}
	if (var->type==&TYPE_STR) {
		const char32_t *str=NULL;
		variable_read(&str, var);

		char32_t *ret=c32sdup(str);
		DIE_IF_MALLOC_FAILS(ret);

		return ret;
	}
	if (var->type==&TYPE_CHAR) {
		char32_t *ret=malloc(sizeof(char32_t) * 2);
		DIE_IF_MALLOC_FAILS(ret);

		variable_read(ret, var);
		ret[1]=U'\0';

		return ret;
	}
	if (var->type==&TYPE_TYPE) {
		type_t *type=NULL;
		variable_read(&type, var);

		char32_t *ret=c32sdup(type->name);
		DIE_IF_MALLOC_FAILS(ret);

		return ret;
	}

	char *tmp=NULL;
	int needed=-1;
	int wrote=-1;

	if (var->type==&TYPE_INT) {
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
	else if (var->type==&TYPE_FLOAT) {
		long double data=0;
		variable_read(&data, var);

		if (isinf(data)) {
			char32_t *ret=NULL;
			if (data < 0.0L) {
				ret=c32sdup(U"-Infinity");
			}
			else {
				ret=c32sdup(U"Infinity");
			}
			DIE_IF_MALLOC_FAILS(ret);
			return ret;
		}

		needed=snprintf(NULL, 0, "%Lf", data) + 1;
		if (needed<0) {
			return NULL;
		}
		tmp=malloc(sizeof(char) * (unsigned long int)needed);
		DIE_IF_MALLOC_FAILS(tmp);

		wrote=snprintf(tmp, (unsigned long int)needed, "%Lf", data);

		//trim excess zeros off of decimal
		size_t len=strlen(tmp);
		for (size_t i=len; i>0; i--) {
			if (tmp[i-1]=='.') {
				tmp[i]='0';
			}
			else if (tmp[i-1]=='0') {
				tmp[i-1]='\0';
				continue;
			}
			break;
		}
	}

	if (wrote<0) {
		free(tmp);
		return NULL;
	}

	char32_t *ret=mbstoc32s(tmp);

	free(tmp);
	return ret;
}

/*
Read variable memory of `var` into `dest`.
*/
void variable_read(void *dest, const variable_t *var) {
	memcpy(dest, var->mem, var->bytes);
}

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const char32_t *type, const char32_t *name, bool is_const) {
	type_t *found_type=find_type(type);

	if (found_type==NULL || !is_valid_identifier_str(name)) {
		return NULL;
	}

	variable_t *var=malloc(sizeof(variable_t));
	DIE_IF_MALLOC_FAILS(var);

	const size_t len=c32slen(name);
	char32_t *name_copy=malloc((len + 1) * sizeof(char32_t));
	DIE_IF_MALLOC_FAILS(name_copy);

	c32slcpy(name_copy, name, len + 1);

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
const char32_t *variable_write(const variable_t *var, const void *data) {
	if (var->is_const) {
		return ERR_CANNOT_ASSIGN_CONST;
	}

	memcpy(var->mem, data, var->bytes);
	return NULL;
}

/*
Free variable `var` and its internal memory.
*/
void free_variable(variable_t *var) {
	if (var!=NULL) {
		free((char32_t*)var->name);
		free(var->mem);
		free(var);
	}
}