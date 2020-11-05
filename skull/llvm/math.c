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
extern Scope *SCOPE;

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

LLVMValueRef llvm_token_to_val(const Variable *, const Token *);

/*
Build LLVM for assigning math operation `oper` from `node` to `var`.
*/
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper *oper) {
	LLVMBuildStore(
		BUILDER,
		oper(
			var,
			llvm_token_to_val(var, node->token),
			llvm_token_to_val(var, node->token->next->next)
		),
		var->alloca
	);
}

/*
Return LLVM equivalent of `token`, checking for compatibility with `var`.
*/
LLVMValueRef llvm_token_to_val(const Variable *var, const Token *token) {
	if (token->token_type != TOKEN_IDENTIFIER) {
		return llvm_parse_var(var, token);
	}

	SCOPE_FIND_VAR(var_found, token, lookup);
	free(lookup);

	if (var->type != var_found->type) {
		PANIC(ERR_TYPE_MISMATCH, { .type = var->type });
	}

	return LLVMBuildLoad2(
		BUILDER,
		var->type->llvm_type(),
		var_found->alloca,
		""
	);
}
