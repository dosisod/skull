#pragma once

#include "./eval_float.h"
#include "./eval_integer.h"

/*
Convert a token to something that can be later assigned to a variable
*/
void *eval_assign(token_t *token, uint8_t *err) {
	void *ret=NULL;
	if (token->token_type==TOKEN_INT_CONST) {
		ret=malloc(sizeof(int64_t));
		int64_t tmp=eval_integer(token, err);
		memcpy(ret, &tmp, sizeof(int64_t));
	}
	else if (token->token_type==TOKEN_FLOAT_CONST) {
		ret=malloc(sizeof(long double));
		long double tmp=eval_float(token, err);
		memcpy(ret, &tmp, sizeof(long double));
	}
	else if (token->token_type==TOKEN_BOOL_CONST) {
		ret=malloc(sizeof(bool));
		bool tmp=token_cmp(L"true", token);
		memcpy(ret, &tmp, sizeof(bool));
	}
	return ret;
}