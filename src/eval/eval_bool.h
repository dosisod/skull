#pragma once

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
int eval_bool_true(token_t *token, const char *code) {
	if (token_cmp("true", token, code)) {
		return EVAL_TRUE;
	}
	if (token_cmp("false", token, code)) {
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
int eval_equality_comparison(token_t *token, const char *code) {
	if ((token->token_type != token->next->next->token_type) || token->token_type==TOKEN_UNKNOWN) {
		return EVAL_ERROR;
	}

	int lhs_len=token_len(token);
	char lhs[lhs_len + 1];
	strlcpy(lhs, code + token->start, lhs_len);

	int rhs_len=token_len(token->next->next);
	char rhs[rhs_len + 1];
	strlcpy(rhs, code + token->next->next->start, rhs_len);

	if (token_cmp("==", token->next, code)) {
		return strcmp(lhs, rhs)==0;
	}
	if (token_cmp("!=", token->next, code)) {
		return strcmp(lhs, rhs)!=0;
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
int eval_bool(const char *code) {
	token_t *token=tokenize(code);
	classify_tokens(token, code);

	if (token==NULL) {
		return EVAL_ERROR;
	}

	int ret;
	if (token->next==NULL) {
		ret=eval_bool_true(token, code);
	}
	else if (token_cmp("not", token, code)) {
		ret=!eval_bool_true(token->next, code);
	}
	else if (token->next->next==NULL) {
		ret=EVAL_ERROR;
	}
	else if (token_cmp("and", token->next, code)) {
		ret=eval_bool_true(token, code) && eval_bool_true(token->next->next, code);
	}
	else if (token_cmp("or", token->next, code)) {
		ret=eval_bool_true(token, code) || eval_bool_true(token->next->next, code);
	}
	else {
		ret=eval_equality_comparison(token, code);
	}

	free(token);
	return ret;
}