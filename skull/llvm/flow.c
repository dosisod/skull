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
#include "skull/llvm/shared.h"
#include "skull/llvm/var.h"

#include "skull/llvm/flow.h"

bool node_to_llvm(AstNode *);

/*
Builds an return statement from `node`.
*/
void gen_stmt_return(AstNode **node) {
	AstNode *const node_val = (*node)->next;
	const bool is_main = SKULL_STATE.current_func == SKULL_STATE.main_func;

	const Expr expr = node_to_expr(NULL, node_val, NULL);

	if (is_main && expr.type != &TYPE_INT) {
		PANIC(ERR_NON_INT_MAIN, { .tok = node_val->token });
	}

	LLVMBuildRet(SKULL_STATE.builder, expr.llvm_value);

	*node = node_val;
}

LLVMValueRef node_to_bool(const AstNode *const);

void gen_control_code_block(
	const char32_t *,
	const AstNode *const,
	LLVMBasicBlockRef
);

/*
Builds LLVM for a while loop from `node`.
*/
void gen_control_while(AstNode **node) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func,
		"while_cond"
	);
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func,
		"while_loop"
	);
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func,
		"while_end"
	);

	LLVMBuildBr(SKULL_STATE.builder, while_cond);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_cond);

	*node = (*node)->next;

	LLVMBuildCondBr(
		SKULL_STATE.builder,
		node_to_bool(*node),
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_loop);

	gen_control_code_block(U"while", *node, while_cond);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_end);
}

void gen_control_if_(AstNode **, LLVMBasicBlockRef, LLVMBasicBlockRef);

/*
Builds an if block from `node`.
*/
void gen_control_if(AstNode **node) {
	gen_control_if_(
		node,
		LLVMGetInsertBlock(SKULL_STATE.builder),
		LLVMAppendBasicBlockInContext(
			SKULL_STATE.ctx,
			SKULL_STATE.current_func,
			"end"
		)
	);
}

/*
Internal function for building an `if` node.
*/
void gen_control_if_(
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

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func,
		"if_true"
	);
	LLVMBasicBlockRef if_false = NULL;
	LLVMMoveBasicBlockBefore(if_true, end);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, if_true);
	*node = (*node)->next;
	gen_control_code_block(U"if", *node, end);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_false = LLVMAppendBasicBlockInContext(
			SKULL_STATE.ctx,
			SKULL_STATE.current_func,
			"if_false"
		);
		LLVMMoveBasicBlockAfter(end, if_false);

		LLVMBuildCondBr(
			SKULL_STATE.builder,
			node_to_bool(*node),
			if_true,
			if_false
		);

		*node = next_non_comment;
	}

	// if there is an elif block following the current if block
	if (next_non_comment && (next_non_comment->type == AST_NODE_ELIF)) {
		gen_control_if_(node, if_false, end);
	}
	// if there is an else block following the current if block
	else if (next_non_comment && (next_non_comment->type == AST_NODE_ELSE)) {
		LLVMPositionBuilderAtEnd(SKULL_STATE.builder, if_false);
		gen_control_code_block(U"else", *node, end);
	}
	// just a single if statement
	else {
		LLVMBuildCondBr(
			SKULL_STATE.builder,
			node_to_bool(*node),
			if_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, end);
}

/*
Try and parse a condition (something returning a bool) from `node`.
*/
LLVMValueRef node_to_bool(const AstNode *const node) {
	const Expr expr = node_to_expr(NULL, node, NULL);

	if (expr.type != &TYPE_BOOL) {
		PANIC(ERR_NON_BOOL_EXPR, { .tok = node->token });
	}

	return expr.llvm_value;
}

/*
Parse `node` while in a new scope. Branch to `block` when done.

`name` is the type of block: if, else, while, etc.
*/
void gen_control_code_block(
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
		const bool returned = node_to_llvm(node->child);

		if (!returned) {
			LLVMBuildBr(SKULL_STATE.builder, block);
		}
	}
	else {
		LLVMBuildBr(SKULL_STATE.builder, block);
	}

	RESTORE_SUB_SCOPE;
}
