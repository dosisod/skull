#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"

#include "skull/llvm/math.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

extern LLVMBuilderRef builder;

/*
Build LLVM for assining addition of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_add(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef add;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		add = LLVMBuildAdd(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		add = LLVMBuildFAdd(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return add;
}

/*
Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_sub(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef sub;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		sub = LLVMBuildSub(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		sub = LLVMBuildFSub(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return sub;
}

/*
Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_mult(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef mult;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		mult = LLVMBuildMul(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		mult = LLVMBuildFMul(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return mult;
}

/*
Build LLVM for assining division of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_div(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef div;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		div = LLVMBuildSDiv(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		div = LLVMBuildFDiv(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return div;
}


/*
Build LLVM for assigning math operation `oper` from `node` to `var`.
*/
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper oper, const char32_t *panic) {
	const Token *lhs = node->token;
	const Token *rhs = node->token->next->next;

	LLVMValueRef result = NULL;

	if (lhs->token_type == rhs->token_type) {
		result = oper(var, lhs, rhs);
	}

	if (!result) {
		PANIC(FMT_ERROR(
			panic,
			{ .tok = lhs },
			{ .tok = rhs }
		));
	}

	LLVMBuildStore(
		builder,
		result,
		var->alloca
	);
}

