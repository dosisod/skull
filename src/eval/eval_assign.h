#pragma once

#include "../common/str.h"
#include "../errors.h"
#include "context.h"
#include "eval_float.h"
#include "eval_integer.h"
#include "variable.h"

#define SETUP_MEM(name, type, func) \
	const type tmp=(func); \
	mem=malloc(sizeof(type)); \
	DIE_IF_MALLOC_FAILS(mem); \
	memcpy((void*)mem, &tmp, sizeof(type));

/*
Assign the value of `token` to a variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.
*/
const char32_t *eval_assign(variable_t *var, token_t *token, const context_t *ctx) {
	if (ctx!=NULL && token->token_type==TOKEN_IDENTIFIER) {
		MAKE_TOKEN_BUF(buf, token);
		variable_t *var_new=context_find_name(ctx, buf);

		if (var_new==NULL) {
			return ERROR_MSG[ERROR_INVALID_INPUT];
		}
		if (var_new->type!=var->type) {
			return ERROR_MSG[ERROR_TYPE_MISMATCH];
		}

		uint8_t mem[var_new->bytes];
		variable_read(mem, var_new);
		variable_write(var, mem);

		return NULL;
	}

	const void *mem=NULL;
	const char32_t *err=NULL;

	if (var->type==find_type(U"int") && token->token_type==TOKEN_INT_CONST) {
		SETUP_MEM(mem, int64_t, eval_integer(token, &err));
	}
	else if (var->type==find_type(U"float") && token->token_type==TOKEN_FLOAT_CONST) {
		SETUP_MEM(mem, long double, eval_float(token, &err));
	}
	else if (var->type==find_type(U"bool") && token->token_type==TOKEN_BOOL_CONST) {
		SETUP_MEM(mem, bool, token_cmp(U"true", token));
	}
	else if (var->type==find_type(U"char") && token->token_type==TOKEN_CHAR_CONST) {
		SETUP_MEM(mem, char32_t, *token->begin);
	}
	else if (var->type==find_type(U"str") && token->token_type==TOKEN_STR_CONST) {
		char32_t *current=NULL;
		variable_read(&current, var);

		if (current!=NULL) {
			free(current);
		}

		MAKE_TOKEN_BUF(buf, token);
		SETUP_MEM(mem, char32_t*, c32sdup(buf));
	}
	else {
		return ERROR_MSG[ERROR_TYPE_MISMATCH];
	}

	if (err!=NULL) {
		free((void*)mem);
		if (var->type==find_type(U"str")) {
			char32_t *str=NULL;
			variable_read(&str, var);
			free(str);
		}
		return err;
	}

	if (variable_write(var, mem)==ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]) {
		free((void*)mem);
		if (var->type==find_type(U"str")) {
			char32_t *str=NULL;
			variable_read(&str, var); // NOLINT
			free(str);
		}
		return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]; // NOLINT
	}

	if (var->type!=find_type(U"str")) {
		free((void*)mem);
	}
	return NULL; // NOLINT
}