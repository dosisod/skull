#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/int.h"
#include "skull/eval/variable.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/flow.h"

extern Scope *SCOPE;
extern LLVMValueRef FUNC;
extern LLVMBuilderRef BUILDER;

bool node_to_llvm_ir(AstNode *);

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode *node) {
	if (node->token->next->token_type == TOKEN_IDENTIFIER) {
		SCOPE_FIND_VAR(found_var, node->token->next, var_name);
		free(var_name);

		if (found_var->type != &TYPE_INT) {
			PANIC("returning non-int variable \"%s\" from main\n", { .var = found_var });
		}

		LLVMBuildRet(
			BUILDER,
			llvm_var_get_value(found_var)
		);
	}
	else {
		LLVMBuildRet(
			BUILDER,
			LLVM_INT(eval_integer(node->token->next))
		);
	}
}

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode *node) {
	LLVMValueRef cond;

	if (node->token->next->token_type == TOKEN_BOOL_CONST) {
		cond = LLVM_BOOL(eval_bool(node->token->next));
	}
	else {
		SCOPE_FIND_VAR(found_var, node->token->next, var_name);

		if (found_var->type != &TYPE_BOOL) {
			PANIC("Expected \"%s\" to be of type bool\n", { .var = found_var });
		}

		cond = llvm_var_get_value(found_var);
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(FUNC, "if_true");
	LLVMBasicBlockRef end = LLVMAppendBasicBlock(FUNC, "end");

	LLVMBuildCondBr(
		BUILDER,
		cond,
		if_true,
		end
	);

	LLVMPositionBuilderAtEnd(
		BUILDER,
		if_true
	);

	Scope *scope_copy = SCOPE;

	SCOPE = make_scope();
	SCOPE->sub_scope = scope_copy;

	if (!node->child) {
		PANIC("if statement must be followed by code block", {0});
	}

	if (node->child->token) {
		const bool returned = node_to_llvm_ir(node->child);

		if (!returned) {
			LLVMBuildBr(BUILDER, end);
		}
	}
	else {
		LLVMBuildBr(BUILDER, end);
	}

	free(SCOPE);
	SCOPE = scope_copy;
	SCOPE->sub_scope = NULL;

	LLVMPositionBuilderAtEnd(
		BUILDER,
		end
	);
}
