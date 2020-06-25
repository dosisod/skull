#include <stddef.h>

#include "../errors.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"

#include "eval_bool.h"

bool eval_bool_true(const token_t *token, const char32_t **error) {
	if (token_cmp(U"true", token)) {
		return true;
	}
	if (token_cmp(U"false", token)) {
		return false;
	}

	*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
	return false;
}

bool eval_equality_comparison(const token_t *token, const char32_t **error) {
	if ((token->token_type != token->next->next->token_type) || token->token_type==TOKEN_UNKNOWN) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return false;
	}

	MAKE_TOKEN_BUF(lhs, token);
	MAKE_TOKEN_BUF(rhs, token->next->next);

	if (token->next->token_type==TOKEN_OPER_EQUAL_EQUAL) {
		return c32scmp(lhs, rhs);
	}
	if (token->next->token_type==TOKEN_OPER_NOT_EQUAL) {
		return !c32scmp(lhs, rhs);
	}

	*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
	return false;
}

bool eval_bool(const token_t *token, const char32_t **error) {
	if (token==NULL) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
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
		err=ERROR_MSG[ERROR_TYPE_MISMATCH];
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