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
	else if (token_cmp("false", token, code)) {
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
```
*/
int eval_bool_equal(token_t *token, const char *code) {
	int lhs=eval_bool_true(token, code);
	int rhs=eval_bool_true(token->next->next, code);

	if (rhs==EVAL_ERROR || lhs==EVAL_ERROR) {
		return EVAL_ERROR;
	}

	if (token_cmp("==", token->next, code)) {
		return lhs==rhs;
	}
	else if (token_cmp("!=", token->next, code)) {
		return lhs!=rhs;
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

	if (token->next==NULL) {
		return eval_bool_true(token, code);
	}
	else if (token_cmp("not", token, code)) {
		return !eval_bool_true(token->next, code);
	}
	else if (token->next->next==NULL) {
		return EVAL_ERROR;
	}
	else if (token_cmp("and", token->next, code)) {
		return eval_bool_true(token, code) && eval_bool_true(token->next->next, code);
	}
	else if (token_cmp("or", token->next, code)) {
		return eval_bool_true(token, code) || eval_bool_true(token->next->next, code);
	}

	return eval_bool_equal(token, code);
}