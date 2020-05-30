#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "../parse/tokenize.h"
#include "../parse/classify.h"

#define EVAL_FALSE 0
#define EVAL_TRUE 1
#define EVAL_ERROR 2

/*
Returns `EVAL_TRUE` or `EVAL_FALSE` if token is `"true"` or `"false"`.

If the token isnt `"true"` or `"false"`, `EVAL_ERROR` is returned instead.
*/
uint8_t eval_bool_true(const token_t *token) {
	if (token_cmp(L"true", token)) {
		return EVAL_TRUE;
	}
	if (token_cmp(L"false", token)) {
		return EVAL_FALSE;
	}

	return EVAL_ERROR;
}

/*
Returns `EVAL_TRUE` or `EVAL_FALSE` if successfull.

If there is an `EVAL_ERROR` on the left or right hand side, `EVAL_ERROR` is returned.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```
*/
uint8_t eval_equality_comparison(const token_t *token) {
	if ((token->token_type != token->next->next->token_type) || token->token_type==TOKEN_UNKNOWN) {
		return EVAL_ERROR;
	}

	MAKE_TOKEN_BUF(lhs, token);
	MAKE_TOKEN_BUF(rhs, token->next->next);

	if (token_cmp(L"==", token->next)) {
		return wcscmp(lhs, rhs)==0;
	}
	if (token_cmp(L"!=", token->next)) {
		return wcscmp(lhs, rhs)!=0;
	}

	return EVAL_ERROR;
}

/*
Evaluates an boolean expression from string.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```
*/
uint8_t eval_bool(const wchar_t *code) {
	token_t *token=tokenize(code);
	classify_tokens(token);

	if (token==NULL) {
		return EVAL_ERROR;
	}

	uint8_t ret;
	if (token->next==NULL) {
		ret=eval_bool_true(token);
	}
	else if (token_cmp(L"not", token)) {
		ret=!eval_bool_true(token->next);
	}
	else if (token->next->next==NULL) {
		ret=EVAL_ERROR;
	}
	else if (token_cmp(L"and", token->next)) {
		ret=eval_bool_true(token) && eval_bool_true(token->next->next);
	}
	else if (token_cmp(L"or", token->next)) {
		ret=eval_bool_true(token) || eval_bool_true(token->next->next);
	}
	else {
		ret=eval_equality_comparison(token);
	}

	free(token);
	return ret;
}