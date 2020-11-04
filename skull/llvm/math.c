#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/math.h"

extern LLVMBuilderRef BUILDER;

/*
Build LLVM for assining addition of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_add(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (var->type == &TYPE_INT) {
		return LLVMBuildAdd(BUILDER, lhs, rhs, "");
	}
	if (var->type == &TYPE_FLOAT) {
		return LLVMBuildFAdd(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_sub(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (var->type == &TYPE_INT) {
		return LLVMBuildSub(BUILDER, lhs, rhs, "");
	}
	if (var->type == &TYPE_FLOAT) {
		return LLVMBuildFSub(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_mult(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (var->type == &TYPE_INT) {
		return LLVMBuildMul(BUILDER, lhs, rhs, "");
	}
	if (var->type == &TYPE_FLOAT) {
		return LLVMBuildFMul(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining division of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_div(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (var->type == &TYPE_INT) {
		return LLVMBuildSDiv(BUILDER, lhs, rhs, "");
	}
	if (var->type == &TYPE_FLOAT) {
		return LLVMBuildFDiv(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assigning math operation `oper` from `node` to `var`.
*/
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper *oper, const char *panic) {
	const Token *lhs = node->token;
	const Token *rhs = node->token->next->next;

	LLVMValueRef result = NULL;

	if (lhs->token_type == rhs->token_type) {
		result = oper(
			var,
			llvm_parse_var(var, lhs),
			llvm_parse_var(var, rhs)
		);
	}

	if (!result) {
		PANIC(
			panic,
			{ .tok = lhs },
			{ .tok = rhs }
		);
	}

	LLVMBuildStore(
		BUILDER,
		result,
		var->alloca
	);
}
