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
#include "skull/llvm/scope.h"
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

void llvm_make_if_wrapper(AstNode *, LLVMBasicBlockRef);

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode **node) {
	LLVMValueRef cond;

	if ((*node)->token->next->token_type == TOKEN_BOOL_CONST) {
		cond = LLVM_BOOL(eval_bool((*node)->token->next));
	}
	else {
		SCOPE_FIND_VAR(found_var, (*node)->token->next, var_name);

		if (found_var->type != &TYPE_BOOL) {
			PANIC("Expected \"%s\" to be of type bool\n", { .var = found_var });
		}

		cond = llvm_var_get_value(found_var);
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(FUNC, "if_true");
	LLVMBasicBlockRef if_false = NULL;

	bool else_follows = (*node)->next && ((*node)->next->node_type == AST_NODE_ELSE);

	if (else_follows) {
		if_false = LLVMAppendBasicBlock(FUNC, "if_false");
	}

	LLVMBasicBlockRef end = LLVMAppendBasicBlock(FUNC, "end");

	LLVMBuildCondBr(
		BUILDER,
		cond,
		if_true,
		else_follows ? if_false : end
	);

	LLVMPositionBuilderAtEnd(
		BUILDER,
		if_true
	);

	llvm_make_if_wrapper(*node, end);

	if (else_follows) {
		LLVMPositionBuilderAtEnd(
			BUILDER,
			if_false
		);

		*node = (*node)->next;
		llvm_make_if_wrapper(*node, end);
	}

	LLVMPositionBuilderAtEnd(
		BUILDER,
		end
	);
}

void llvm_make_if_wrapper(AstNode *node, LLVMBasicBlockRef block) {
	MAKE_SUB_SCOPE;

	if (!node->child) {
		PANIC("if statement must be followed by code block", {0});
	}
	if (node->child->token) {
		const bool returned = node_to_llvm_ir(node->child);

		if (!returned) {
			LLVMBuildBr(BUILDER, block);
		}
	}
	else {
		LLVMBuildBr(BUILDER, block);
	}

	RESTORE_SUB_SCOPE;
}
