#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/llvm/aliases.h"
#include "skull/parse/classify.h"

#include "skull/llvm/math.h"

extern LLVMBuilderRef builder;

/*
Build LLVM for assining addition of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_add(Variable *var, const Token *lhs, const Token *rhs) {
	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		return LLVMBuildAdd(
			builder,
			LLVM_INT(eval_integer(lhs)),
			LLVM_INT(eval_integer(rhs)),
			""
		);
	}
	if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		return LLVMBuildFAdd(
			builder,
			LLVM_FLOAT(eval_float(lhs)),
			LLVM_FLOAT(eval_float(rhs)),
			""
		);
	}

	return NULL;
}

/*
Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_sub(Variable *var, const Token *lhs, const Token *rhs) {
	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		return LLVMBuildSub(
			builder,
			LLVM_INT(eval_integer(lhs)),
			LLVM_INT(eval_integer(rhs)),
			""
		);
	}
	if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		return LLVMBuildFSub(
			builder,
			LLVM_FLOAT(eval_float(lhs)),
			LLVM_FLOAT(eval_float(rhs)),
			""
		);
	}

	return NULL;
}

/*
Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_mult(Variable *var, const Token *lhs, const Token *rhs) {
	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		return LLVMBuildMul(
			builder,
			LLVM_INT(eval_integer(lhs)),
			LLVM_INT(eval_integer(rhs)),
			""
		);
	}
	if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		return LLVMBuildFMul(
			builder,
			LLVM_FLOAT(eval_float(lhs)),
			LLVM_FLOAT(eval_float(rhs)),
			""
		);
	}

	return NULL;
}

/*
Build LLVM for assining division of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_div(Variable *var, const Token *lhs, const Token *rhs) {
	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		return LLVMBuildSDiv(
			builder,
			LLVM_INT(eval_integer(lhs)),
			LLVM_INT(eval_integer(rhs)),
			""
		);
	}
	if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		return LLVMBuildFDiv(
			builder,
			LLVM_FLOAT(eval_float(lhs)),
			LLVM_FLOAT(eval_float(rhs)),
			""
		);
	}

	return NULL;
}


/*
Build LLVM for assigning math operation `oper` from `node` to `var`.
*/
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper oper, const char *panic) {
	const Token *lhs = node->token;
	const Token *rhs = node->token->next->next;

	LLVMValueRef result = NULL;

	if (lhs->token_type == rhs->token_type) {
		result = oper(var, lhs, rhs);
	}

	if (!result) {
		PANIC(
			panic,
			{ .tok = lhs },
			{ .tok = rhs }
		);
	}

	LLVMBuildStore(
		builder,
		result,
		var->alloca
	);
}

