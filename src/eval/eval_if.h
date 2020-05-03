#define EVAL_IF_FALSE 0
#define EVAL_IF_TRUE 1
#define EVAL_IF_ERROR 2

/*
Returns `EVAL_IF_TRUE` or `EVAL_IF_FALSE` if token is `"true"` or `"false"`.

If the token isnt `"true"` or `"false"`, `EVAL_TOKEN_ERROR` is returned instead.
*/
int eval_if_true(token_t *token, const char *code) {
	if (token_cmp("true", token, code)) {
		return EVAL_IF_TRUE;
	}
	else if (token_cmp("false", token, code)) {
		return EVAL_IF_FALSE;
	}

	return EVAL_IF_ERROR;
}

/*
Returns an `EVAL_IF` value for an equality comparison.

If there is an `EVAL_IF_ERROR` on the left or right hand side, `EVAL_IF_ERROR` is returned.

Examples include:

```
"true == false"
"true != true"
```
*/
int eval_if_equal(token_t *token, const char *code) {
	int lhs=eval_if_true(token, code);
	int rhs=eval_if_true(token->next->next, code);

	if (rhs==EVAL_IF_ERROR || lhs==EVAL_IF_ERROR) {
		return EVAL_IF_ERROR;
	}

	if (token_cmp("==", token->next, code)) {
		return lhs==rhs;
	}
	else if (token_cmp("!=", token->next, code)) {
		return lhs!=rhs;
	}

	return EVAL_IF_ERROR;
}

/*
Evaluates an if statement in string form.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```
*/
int eval_if(const char *code) {
	token_t *token=tokenize(code);
	classify_tokens(token, code);

	if (token->next==NULL) {
		return eval_if_true(token, code);
	}
	else if (token_cmp("not", token, code)) {
		return !eval_if_true(token->next, code);
	}
	else if (token->next->next==NULL) {
		return EVAL_IF_ERROR;
	}

	return eval_if_equal(token, code);
}