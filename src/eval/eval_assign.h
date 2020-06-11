#pragma once

#define _GNU_SOURCE

#include <wchar.h>

#include "../errors.h"
#include "./eval_float.h"
#include "./eval_integer.h"
#include "./variable.h"

/*
Assign the value of `token` to a variable `var`.

Return an error (as a string) if any occured, else `NULL`.
*/
const wchar_t *eval_assign(variable_t *var, token_t *token) {
	const void *mem=NULL;
	const wchar_t *err=NULL;

	if (var->type==find_type(L"int") && token->token_type==TOKEN_INT_CONST) {
		int64_t tmp=eval_integer(token, &err);
		mem=&tmp;
	}
	else if (var->type==find_type(L"float") && token->token_type==TOKEN_FLOAT_CONST) {
		long double tmp=eval_float(token, &err);
		mem=&tmp;
	}
	else if (var->type==find_type(L"bool") && token->token_type==TOKEN_BOOL_CONST) {
		bool tmp=token_cmp(L"true", token);
		mem=&tmp;
	}
	else if (var->type==find_type(L"char") && token->token_type==TOKEN_CHAR_CONST) {
		mem=token->begin;
	}
	else if (var->type==find_type(L"str") && token->token_type==TOKEN_STR_CONST) {
		wchar_t *current=NULL;
		variable_read(&current, var);

		if (current!=NULL) {
			free(current);
		}

		MAKE_TOKEN_BUF(buf, token);
		wchar_t *tmp=wcsdup(buf);
		mem=&tmp;
	}
	else {
		return ERROR_MSG[ERROR_TYPE_MISMATCH];
	}

	if (err!=NULL) {
		if (var->type==find_type(L"str")) {
			mem=NULL;
			variable_read(&mem, var);
			free((wchar_t*)mem);
		}
		return err;
	}
	if (variable_write(var, mem)==VARIABLE_WRITE_ECONST) {
		if (var->type==find_type(L"str")) {
			mem=NULL;
			variable_read(&mem, var);
			free((wchar_t*)mem);
		}
		return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]; // NOLINT
	}
	return NULL;
}