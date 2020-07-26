#include <stddef.h>

#include "skull/errors.h"
#include "skull/parse/classify.h"
#include "skull/parse/tokenize.h"

#include "skull/eval/eval_bool.h"

/*
Resolve a boolean expression from `token` into `true` or `false`.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

`error` is set to `NULL` if no error occurs, else `error` points to error msg.
*/
bool eval_bool(const token_t *token, const char32_t **error) {
	if (token==NULL) {
		*error=ERR_TYPE_MISMATCH;
		return false;
	}

	bool ret=false;
	const char32_t *err=NULL;
	if (token->next==NULL) {
		ret=eval_bool_true(token, &err);
	}
	else if (token->token_type==TOKEN_KW_NOT) {
		ret=!eval_bool_true(token->next, &err);
	}
	else if (token->next->next==NULL) {
		err=ERR_MISSING_TOKEN;
	}
	else if (token->next->token_type==TOKEN_KW_AND) {
		ret=(
			eval_bool_true(token, &err) &&
			eval_bool_true(token->next->next, &err)
		);
	}
	else if (token->next->token_type==TOKEN_KW_OR) {
		ret=(
			eval_bool_true(token, &err) ||
			eval_bool_true(token->next->next, &err)
		);
	}
	else {
		ret=eval_equality_comparison(token, &err);
	}

	*error=err;
	return ret;
}

/*
Return the result of an equality comparison pointed to by `token`.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

`error` is set to `NULL` if no error occurs, else `error` points to error msg.
*/
bool eval_equality_comparison(const token_t *token, const char32_t **error) {
	if ((token->token_type != token->next->next->token_type) || token->token_type==TOKEN_UNKNOWN) {
		*error=ERR_TYPE_MISMATCH;
		return false;
	}

	MAKE_TOKEN_BUF(lhs, token);
	MAKE_TOKEN_BUF(rhs, token->next->next);

	const bool ret=c32scmp(lhs, rhs);

	if (token->next->token_type==TOKEN_OPER_EQUAL_EQUAL) {
		return ret;
	}
	if (token->next->token_type==TOKEN_OPER_NOT_EQUAL) {
		return !ret;
	}

	*error=ERR_COMPARISON_BAD;
	return false;
}

/*
Returns `true` or `false` if `token` is `"true"` or `"false"`.

`error` is set to `NULL` if no error occurs, else `error` points to error msg.
*/
bool eval_bool_true(const token_t *token, const char32_t **error) {
	if (token_cmp(U"true", token)) {
		return true;
	}
	if (token_cmp(U"false", token)) {
		return false;
	}

	*error=ERR_BOOL_BAD;
	return false;
}