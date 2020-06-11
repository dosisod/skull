#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "../errors.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"

/*
Returns `true` or `false` if token is `"true"` or `"false"`.

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
bool eval_bool_true(const token_t *token, const wchar_t **error) {
	if (token_cmp(L"true", token)) {
		return true;
	}
	if (token_cmp(L"false", token)) {
		return false;
	}

	*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
	return false;
}

/*
Return the result of an equality comparison.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

If an error occurs on either side, `error` is set to non `NULL`.
*/
bool eval_equality_comparison(const token_t *token, const wchar_t **error) {
	if ((token->token_type != token->next->next->token_type) || token->token_type==TOKEN_UNKNOWN) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return false;
	}

	MAKE_TOKEN_BUF(lhs, token);
	MAKE_TOKEN_BUF(rhs, token->next->next);

	if (token_cmp(L"==", token->next)) {
		return wcscmp(lhs, rhs)==0;
	}
	if (token_cmp(L"!=", token->next)) {
		return wcscmp(lhs, rhs)!=0;
	}

	*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
	return false;
}

/*
Resolve a boolean expression from string into `true` or `false`.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

If an error an occurs, `error` is set to non `NULL`.
*/
bool eval_bool(const wchar_t *code, const wchar_t **error) {
	token_t *token=tokenize(code);
	classify_tokens(token);

	if (token==NULL) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return false;
	}

	bool ret=false;
	const wchar_t *err=NULL;
	if (token->next==NULL) {
		ret=eval_bool_true(token, &err);
	}
	else if (token_cmp(L"not", token)) {
		ret=!eval_bool_true(token->next, &err);
	}
	else if (token->next->next==NULL) {
		err=ERROR_MSG[ERROR_TYPE_MISMATCH];
	}
	else if (token_cmp(L"and", token->next)) {
		ret=(
			eval_bool_true(token, &err) &&
			eval_bool_true(token->next->next, &err)
		);
	}
	else if (token_cmp(L"or", token->next)) {
		ret=(
			eval_bool_true(token, &err) ||
			eval_bool_true(token->next->next, &err)
		);
	}
	else {
		ret=eval_equality_comparison(token, &err);
	}

	*error=err;
	free(token);
	return ret;
}