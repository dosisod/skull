#include <stdlib.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "../errors.h"
#include "../parse/classify.h"
#include "eval_float.h"
#include "eval_integer.h"

#include "eval_assign.h"

#define SETUP_MEM(name, type, func) \
	const type tmp=(func); \
	mem=malloc(sizeof(type)); \
	DIE_IF_MALLOC_FAILS(mem); \
	memcpy((void*)mem, &tmp, sizeof(type));

const char32_t *eval_assign(variable_t *var, token_t *token, const context_t *ctx) {
	if (token==NULL) {
		return ERROR_MSG[ERROR_INVALID_INPUT];
	}

	if (ctx!=NULL && token->token_type==TOKEN_IDENTIFIER) {
		MAKE_TOKEN_BUF(buf, token);
		variable_t *var_found=context_find_name(ctx, buf);

		if (var_found==NULL) {
			return ERROR_MSG[ERROR_INVALID_INPUT];
		}
		if (var_found->type!=var->type) {
			return ERROR_MSG[ERROR_TYPE_MISMATCH];
		}

		uint8_t mem[var_found->bytes];
		variable_read(mem, var_found);

		if (var->type==&TYPE_STR) {
			char32_t *str=NULL;
			variable_read(&str, var_found);

			str=c32sdup(str);
			memcpy(mem, &str, sizeof(char32_t*));
		}
		variable_write(var, mem);

		return NULL;
	}

	const void *mem=NULL;
	const char32_t *err=NULL;

	if (var->type==&TYPE_INT && token->token_type==TOKEN_INT_CONST) {
		SETUP_MEM(mem, int64_t, eval_integer(token, &err));
	}
	else if (var->type==&TYPE_FLOAT && token->token_type==TOKEN_FLOAT_CONST) {
		SETUP_MEM(mem, long double, eval_float(token, &err));
	}
	else if (var->type==&TYPE_BOOL && token->token_type==TOKEN_BOOL_CONST) {
		SETUP_MEM(mem, bool, token_cmp(U"true", token));
	}
	else if (var->type==&TYPE_CHAR && token->token_type==TOKEN_CHAR_CONST) {
		SETUP_MEM(mem, char32_t, *token->begin);
	}
	else if (var->type==&TYPE_STR && token->token_type==TOKEN_STR_CONST) {
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
		if (var->type==&TYPE_STR) {
			char32_t *str=NULL;
			variable_read(&str, var);
			free(str);
		}
		return err;
	}

	if (variable_write(var, mem)==ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]) {
		free((void*)mem);
		if (var->type==&TYPE_STR) {
			char32_t *str=NULL;
			variable_read(&str, var); // NOLINT
			free(str);
		}
		return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]; // NOLINT
	}

	if (var->type!=&TYPE_STR) {
		free((void*)mem);
	}
	return NULL; // NOLINT
}

#undef SETUP_MEM