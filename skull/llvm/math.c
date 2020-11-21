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
Build LLVM for assining addition of `lhs` and `rhs`.
*/
LLVMValueRef llvm_make_add(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return LLVMBuildAdd(BUILDER, lhs, rhs, "");
	}
	if (type == &TYPE_FLOAT) {
		return LLVMBuildFAdd(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining subtraction of `lhs` and `rhs`.
*/
LLVMValueRef llvm_make_sub(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return LLVMBuildSub(BUILDER, lhs, rhs, "");
	}
	if (type == &TYPE_FLOAT) {
		return LLVMBuildFSub(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining multiplication of `lhs` and `rhs`.
*/
LLVMValueRef llvm_make_mult(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return LLVMBuildMul(BUILDER, lhs, rhs, "");
	}
	if (type == &TYPE_FLOAT) {
		return LLVMBuildFMul(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

/*
Build LLVM for assining division of `lhs` and `rhs`.
*/
LLVMValueRef llvm_make_div(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			PANIC("division by zero", {0});
		}

		return LLVMBuildSDiv(BUILDER, lhs, rhs, "");
	}
	if (type == &TYPE_FLOAT) {
		return LLVMBuildFDiv(BUILDER, lhs, rhs, "");
	}

	return NULL;
}

LLVMValueRef llvm_token_to_val(const Type *, const Token *);

/*
Return LLVM for assigning math operation `oper` from `node`.
*/
LLVMValueRef llvm_make_math_oper(const Type *type, const AstNode *node, MathOper *oper) {
	return oper(
		type,
		llvm_token_to_val(type, ATTR(AstNodeOper, node, lhs)),
		llvm_token_to_val(type, ATTR(AstNodeOper, node, rhs))
	);
}

/*
Return LLVM equivalent of `token`, checking for compatibility with `type`.
*/
LLVMValueRef llvm_token_to_val(const Type *type, const Token *token) {
	if (token->token_type != TOKEN_IDENTIFIER) {
		return llvm_parse_var(type, token);
	}

	SCOPE_FIND_VAR(var_found, token, lookup);
	free(lookup);

	if (type != var_found->type) {
		PANIC(ERR_TYPE_MISMATCH, { .type = type });
	}

	return llvm_var_get_value(var_found);
}
