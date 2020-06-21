#pragma once

#include "../common/str.h"
#include "../errors.h"
#include "context.h"
#include "eval_float.h"
#include "eval_integer.h"
#include "variable.h"

/*
Assign the value of `token` to a variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.
*/
const char32_t *eval_assign(variable_t *var, token_t *token, const context_t *ctx) {
	const void *mem=NULL;
	const char32_t *err=NULL;

	if (ctx!=NULL && token->token_type==TOKEN_IDENTIFIER) {
		MAKE_TOKEN_BUF(buf, token);
		variable_t *var_new=context_find_name(ctx, buf);

		if (var_new==NULL) {
			return ERROR_MSG[ERROR_INVALID_INPUT];
		}
		if (var_new->type!=var->type) {
			return ERROR_MSG[ERROR_TYPE_MISMATCH];
		}

		uint8_t mem_tmp[var_new->bytes];
		variable_read(mem_tmp, var_new);
		variable_write(var, mem_tmp);

		return NULL;
	}

	if (var->type==find_type(U"int") && token->token_type==TOKEN_INT_CONST) {
		int64_t tmp=eval_integer(token, &err);
		mem=&tmp;
	}
	else if (var->type==find_type(U"float") && token->token_type==TOKEN_FLOAT_CONST) {
		long double tmp=eval_float(token, &err);
		mem=&tmp;
	}
	else if (var->type==find_type(U"bool") && token->token_type==TOKEN_BOOL_CONST) {
		bool tmp=token_cmp(U"true", token);
		mem=&tmp;
	}
	else if (var->type==find_type(U"char") && token->token_type==TOKEN_CHAR_CONST) {
		mem=token->begin;
	}
	else if (var->type==find_type(U"str") && token->token_type==TOKEN_STR_CONST) {
		char32_t *current=NULL;
		variable_read(&current, var);

		if (current!=NULL) {
			free(current);
		}

		MAKE_TOKEN_BUF(buf, token);
		char32_t *tmp=c32sdup(buf);
		DIE_IF_MALLOC_FAILS(tmp);

		mem=&tmp;
	}
	else {
		return ERROR_MSG[ERROR_TYPE_MISMATCH];
	}

	if (err!=NULL) {
		if (var->type==find_type(U"str")) {
			mem=NULL;
			variable_read(&mem, var);
			free((char32_t*)mem);
		}
		return err;
	}
	if (variable_write(var, mem)==ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]) {
		if (var->type==find_type(U"str")) {
			mem=NULL;
			variable_read(&mem, var); // NOLINT
			free((char32_t*)mem);
		}
		return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]; // NOLINT
	}
	return NULL; // NOLINT
}