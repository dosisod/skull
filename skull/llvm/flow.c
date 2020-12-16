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
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern LLVMBuilderRef BUILDER;

bool node_to_llvm_ir(AstNode *);

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode *node) {
	const Token *const token_val = node->token->next;
	if (token_val->token_type == TOKEN_IDENTIFIER) {
		SCOPE_FIND_VAR(found_var, token_val, var_name);
		free(var_name);

		if (CURRENT_FUNC == MAIN_FUNC && found_var->type != &TYPE_INT) {
			PANIC(ERR_NON_INT_VAR_MAIN, { .var = found_var });
		}

		LLVMBuildRet(BUILDER, llvm_var_get_value(found_var));
		return;
	}

	if (CURRENT_FUNC == MAIN_FUNC && token_val->token_type != TOKEN_INT_CONST) {
		PANIC(ERR_NON_INT_VAL_MAIN, { .tok = token_val });
	}

	LLVMBuildRet(BUILDER, llvm_parse_token(token_val));
}

LLVMValueRef llvm_make_cond(AstNode *);
void llvm_make_code_block(const char32_t *, AstNode *, LLVMBasicBlockRef);

/*
Builds LLVM for a while loop from `node`.
*/
void llvm_make_while(AstNode *node) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlock(CURRENT_FUNC, "while_cond");
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlock(CURRENT_FUNC, "while_loop");
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlock(CURRENT_FUNC, "while_end");

	LLVMBuildBr(BUILDER, while_cond);

	LLVMPositionBuilderAtEnd(BUILDER, while_cond);

	LLVMBuildCondBr(
		BUILDER,
		llvm_make_cond(node),
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(BUILDER, while_loop);

	llvm_make_code_block(U"while", node, while_cond);

	LLVMPositionBuilderAtEnd(BUILDER, while_end);
}

void llvm_make_if_(AstNode **, LLVMBasicBlockRef, LLVMBasicBlockRef);

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode **node) {
	llvm_make_if_(
		node,
		LLVMGetInsertBlock(BUILDER),
		LLVMAppendBasicBlock(CURRENT_FUNC, "end")
	);
}

/*
Internal function for building an `if` node.
*/
void llvm_make_if_(AstNode **node, LLVMBasicBlockRef entry, LLVMBasicBlockRef end) {
	AstNode *skip_whitespace = (*node)->next;
	while (skip_whitespace) {
		if (skip_whitespace->node_type == AST_NODE_COMMENT) {
			skip_whitespace = skip_whitespace->next;
			continue;
		}
		break;
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(CURRENT_FUNC, "if_true");
	LLVMBasicBlockRef if_false = NULL;
	LLVMMoveBasicBlockBefore(if_true, end);

	LLVMPositionBuilderAtEnd(BUILDER, if_true);
	llvm_make_code_block(U"if", *node, end);

	LLVMPositionBuilderAtEnd(BUILDER, entry);

	if (skip_whitespace && (
		skip_whitespace->node_type == AST_NODE_ELIF ||
		skip_whitespace->node_type == AST_NODE_ELSE)
	) {
		if_false = LLVMAppendBasicBlock(CURRENT_FUNC, "if_false");
		LLVMMoveBasicBlockAfter(end, if_false);

		LLVMBuildCondBr(
			BUILDER,
			llvm_make_cond(*node),
			if_true,
			if_false
		);

		*node = skip_whitespace;
	}

	// if there is an elif block following the current if block
	if (skip_whitespace && (skip_whitespace->node_type == AST_NODE_ELIF)) {
		llvm_make_if_(node, if_false, end);
	}
	// if there is an else block following the current if block
	else if (skip_whitespace && (skip_whitespace->node_type == AST_NODE_ELSE)) {
		LLVMPositionBuilderAtEnd(BUILDER, if_false);
		llvm_make_code_block(U"else", *node, end);
	}
	else {
		LLVMBuildCondBr(
			BUILDER,
			llvm_make_cond(*node),
			if_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(BUILDER, end);
}

/*
Try and parse a condition (something returning a bool) from `node`.
*/
LLVMValueRef llvm_make_cond(AstNode *node) {
	if (node->token->next->token_type == TOKEN_BOOL_CONST) {
		return LLVM_BOOL(eval_bool(node->token->next));
	}
	SCOPE_FIND_VAR(found_var, node->token->next, var_name);

	if (found_var->type != &TYPE_BOOL) {
		PANIC(ERR_NON_BOOL_COND, { .var = found_var });
	}

	return llvm_var_get_value(found_var);
}

/*
Parse `node` while in a new scope. Branch to `block` when done.

`name` is the type of block: if, else, while, etc.
*/
void llvm_make_code_block(const char32_t *name, AstNode *node, LLVMBasicBlockRef block) {
	MAKE_SUB_SCOPE;

	if (!node->child) {
		PANIC(ERR_MISSING_BLOCK, { .str = name });
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
