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
#include "skull/llvm/assign.h"
#include "skull/llvm/oper.h"
#include "skull/llvm/scope.h"
#include "skull/llvm/var.h"

#include "skull/llvm/flow.h"

extern Scope *SCOPE;
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern LLVMBuilderRef BUILDER;

bool node_to_llvm_ir(AstNode *);

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode **node) {
	AstNode *const node_val = (*node)->next;
	const bool is_main = CURRENT_FUNC == MAIN_FUNC;

	const Expr expr = node_to_expr(NULL, node_val, NULL);

	if (is_main && expr.type != &TYPE_INT) {
		PANIC(ERR_NON_INT_MAIN, { .tok = node_val->token });
	}

	LLVMBuildRet(BUILDER, expr.llvm_value);

	*node = node_val;
}

LLVMValueRef llvm_make_cond(const AstNode *const);
void llvm_make_code_block(
	const char32_t *,
	const AstNode *const,
	LLVMBasicBlockRef
);

/*
Builds LLVM for a while loop from `node`.
*/
void llvm_make_while(AstNode **node) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlock(
		CURRENT_FUNC,
		"while_cond"
	);
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlock(
		CURRENT_FUNC,
		"while_loop"
	);
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlock(
		CURRENT_FUNC,
		"while_end"
	);

	LLVMBuildBr(BUILDER, while_cond);

	LLVMPositionBuilderAtEnd(BUILDER, while_cond);

	*node = (*node)->next;

	LLVMBuildCondBr(
		BUILDER,
		llvm_make_cond(*node),
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(BUILDER, while_loop);

	llvm_make_code_block(U"while", *node, while_cond);

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
void llvm_make_if_(
	AstNode **node,
	LLVMBasicBlockRef entry,
	LLVMBasicBlockRef end
) {
	AstNode *next_non_comment = (*node)->next;

	while (next_non_comment) {
		if (next_non_comment->type != AST_NODE_ELIF &&
			next_non_comment->type != AST_NODE_ELSE
		) {
			next_non_comment = next_non_comment->next;
			continue;
		}
		break;
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(CURRENT_FUNC, "if_true");
	LLVMBasicBlockRef if_false = NULL;
	LLVMMoveBasicBlockBefore(if_true, end);

	LLVMPositionBuilderAtEnd(BUILDER, if_true);
	*node = (*node)->next;
	llvm_make_code_block(U"if", *node, end);

	LLVMPositionBuilderAtEnd(BUILDER, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_false = LLVMAppendBasicBlock(CURRENT_FUNC, "if_false");
		LLVMMoveBasicBlockAfter(end, if_false);

		LLVMBuildCondBr(
			BUILDER,
			llvm_make_cond(*node),
			if_true,
			if_false
		);

		*node = next_non_comment;
	}

	// if there is an elif block following the current if block
	if (next_non_comment && (next_non_comment->type == AST_NODE_ELIF)) {
		llvm_make_if_(node, if_false, end);
	}
	// if there is an else block following the current if block
	else if (next_non_comment && (next_non_comment->type == AST_NODE_ELSE)) {
		LLVMPositionBuilderAtEnd(BUILDER, if_false);
		llvm_make_code_block(U"else", *node, end);
	}
	// just a single if statement
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

LLVMValueRef llvm_get_bool_from_token(const Token *);

/*
Try and parse a condition (something returning a bool) from `node`.
*/
LLVMValueRef llvm_make_cond(const AstNode *const node) {
	const Expr expr = node_to_expr(NULL, node, NULL);

	if (expr.type != &TYPE_BOOL) {
		PANIC(ERR_NON_BOOL_EXPR, { .tok = node->token });
	}

	return expr.llvm_value;
}

/*
Returns an LLVM value parsed from `token`.
*/
LLVMValueRef llvm_get_bool_from_token(const Token *token) {
	const Expr expr = token_to_expr(token, NULL);

	if (expr.type != &TYPE_BOOL) {
		PANIC(ERR_NON_BOOL_EXPR, { .tok = token });
	}

	return expr.llvm_value;
}

/*
Parse `node` while in a new scope. Branch to `block` when done.

`name` is the type of block: if, else, while, etc.
*/
void llvm_make_code_block(
	const char32_t *name,
	const AstNode *const node,
	LLVMBasicBlockRef block
) {
	if (!node->child) { // NOLINT
		PANIC(ERR_MISSING_BLOCK, {
			.tok = node->token,
			.str = name
		});
	}

	MAKE_SUB_SCOPE;

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
