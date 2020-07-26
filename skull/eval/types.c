#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/malloc.h"

#include "skull/eval/types.h"
#include "skull/eval/variable.h"

/*
Creates a new type called `name` with `bytes` bytes of memory.

Returns false if a type called `name` already exists, and was not inserted.
Returns true if the type `name` was inserted.
*/
bool make_new_type(const char32_t *name, size_t bytes) {
	type_t *current=TYPES_AVAILABLE;
	type_t *last=current;

	while (current!=NULL) {
		if (c32scmp(current->name, name)) {
			return false;
		}
		last=current;
		current=current->next;
	}

	type_t *new_type=malloc(sizeof(type_t));
	DIE_IF_MALLOC_FAILS(new_type);

	new_type->name=name;
	new_type->bytes=bytes;
	new_type->next=NULL;

	last->next=new_type; // NOLINT

	return true;
}

/*
Returns pointer to type with name `name`.
*/
type_t *find_type(const char32_t *name) {
	type_t *head=TYPES_AVAILABLE;

	while (head!=NULL) {
		if (c32scmp(name, head->name)) {
			return head;
		}
		head=head->next;
	}

	return NULL;
}

/*
Free all non-builtin types.
*/
void free_types(void) {
	//TYPE_BOOL is the last defined builtin type
	type_t *head=TYPE_BOOL.next;
	type_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}

	TYPE_BOOL.next=NULL;
}

char32_t *fmt_bool_type(const variable_t *var) {
	bool data=false;
	variable_read(&data, var);

	char32_t *ret=c32sdup(data ? U"true" : U"false");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

struct type_t TYPE_BOOL = {
	.name=U"bool",
	.bytes=sizeof(bool),
	.to_string=&fmt_bool_type,
	.next=NULL
};

#define SPRINTF_FMT(fmt) \
	int needed=snprintf(NULL, 0, (fmt), data) + 1; \
	if (needed<0) { \
		return NULL; \
	} \
	char *tmp=malloc(sizeof(char) * (unsigned long int)needed); \
	DIE_IF_MALLOC_FAILS(tmp); \
	int wrote=snprintf(tmp, (unsigned long int)needed, (fmt), data); \
	if (wrote<0) { \
		free(tmp); \
		return NULL; \
	} \

char32_t *fmt_int_type(const variable_t *var) {
	int64_t data=0;
	variable_read(&data, var);

	SPRINTF_FMT("%li");

	char32_t *ret=mbstoc32s(tmp);
	free(tmp);
	return ret;
}

struct type_t TYPE_INT = {
	.name=U"int",
	.bytes=sizeof(int64_t),
	.to_string=&fmt_int_type,
	.next=&TYPE_BOOL
};

char32_t *fmt_float_type(const variable_t *var) {
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

	SPRINTF_FMT("%Lf");

	//trim excess zeros off of decimal
	for (size_t i=strlen(tmp); i>0; i--) {
		if (tmp[i-1]=='.') {
			tmp[i]='0';
		}
		else if (tmp[i-1]=='0') {
			tmp[i-1]='\0';
			continue;
		}
		break;
	}

	char32_t *ret=mbstoc32s(tmp);
	free(tmp);
	return ret;
}

#undef SPRINTF_FMT

struct type_t TYPE_FLOAT = {
	.name=U"float",
	.bytes=sizeof(long double),
	.to_string=&fmt_float_type,
	.next=&TYPE_INT
};

char32_t *fmt_char_type(const variable_t *var) {
	char32_t *ret=malloc(sizeof(char32_t) * 2);
	DIE_IF_MALLOC_FAILS(ret);

	variable_read(ret, var);
	ret[1]=U'\0';

	return ret;
}

struct type_t TYPE_CHAR = {
	.name=U"char",
	.bytes=sizeof(char32_t),
	.to_string=&fmt_char_type,
	.next=&TYPE_FLOAT
};

char32_t *fmt_str_type(const variable_t *var) {
	const char32_t *str=NULL;
	variable_read(&str, var);

	char32_t *ret=c32sdup(str);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

struct type_t TYPE_STR = {
	.name=U"str",
	.bytes=sizeof(char32_t*),
	.to_string=&fmt_str_type,
	.next=&TYPE_CHAR
};

char32_t *fmt_type_type(const variable_t *var) {
	type_t *type=NULL;
	variable_read(&type, var);

	char32_t *ret=c32sdup(type->name);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

struct type_t TYPE_TYPE = {
	.name=U"type",
	.bytes=sizeof(struct type_t*),
	.to_string=&fmt_type_type,
	.next=&TYPE_STR
};

struct type_t *TYPES_AVAILABLE=&TYPE_TYPE;
