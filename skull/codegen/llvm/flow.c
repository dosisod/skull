#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/core.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/codegen/llvm/flow.h"

static void gen_control_if_(
	const AstNode **,
	LLVMBasicBlockRef,
	LLVMBasicBlockRef,
	SkullStateLLVM *
);

static void gen_control_code_block(
	const AstNode *,
	LLVMBasicBlockRef,
	SkullStateLLVM *
);


/*
Build an unreachable statement.
*/
Expr gen_stmt_unreachable(const SkullStateLLVM *state) {
	return (Expr){
		.value = LLVMBuildUnreachable(state->builder),
		.type = &TYPE_VOID
	};
}

void gen_stmt_implicit_main_return(
	const Location *location,
	const SkullStateLLVM *state
) {
	LLVMValueRef ret = LLVMBuildRet(state->builder, LLVM_INT(0));

	add_llvm_debug_info(ret, location, state);
}

void gen_stmt_noop(const Location *location, const SkullStateLLVM *state) {
	if (!BUILD_DATA.debug) return;

	LLVMValueRef noop = LLVMBuildStore(
		state->builder,
		LLVM_INT(0),
		LLVMBuildAlloca(
			state->builder,
			type_to_llvm_type(&TYPE_INT, state),
			"noop"
		)
	);

	add_llvm_debug_info(noop, location, state);
}

/*
Builds an return statement from `node`.
*/
Expr gen_stmt_return(const AstNode *node, const SkullStateLLVM *state) {
	if (!node->expr) {
		return (Expr){
			.value = LLVMBuildRetVoid(state->builder),
			.type = &TYPE_VOID
		};
	}

	const Expr expr = gen_expr(node->expr, state);

	LLVMValueRef ret = LLVMBuildRet(state->builder, expr.value);

	add_llvm_debug_info(ret, &node->token->location, state);

	return expr;
}

/*
Builds LLVM for a while loop from `node`.
*/
void gen_control_while(const AstNode *node, SkullStateLLVM *state) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlockInContext(
		state->ctx,
		state->current_func->ref,
		"while_cond"
	);
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlockInContext(
		state->ctx,
		state->current_func->ref,
		"while_loop"
	);
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlockInContext(
		state->ctx,
		state->current_func->ref,
		"while_end"
	);

	state->current_while_cond = while_cond;
	state->current_while_end = while_end;

	LLVMBuildBr(state->builder, while_cond);
	LLVMPositionBuilderAtEnd(state->builder, while_cond);

	LLVMValueRef cond = gen_expr(node->expr, state).value;
	add_llvm_debug_info(cond, find_expr_node_location(node->expr), state);

	LLVMBuildCondBr(
		state->builder,
		cond,
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(state->builder, while_loop);
	gen_control_code_block(node, while_cond, state);
	LLVMPositionBuilderAtEnd(state->builder, while_end);

	// shouldn't be needed since semantic layer checks that all break/continue
	// statements are in a while loop, but it doesn't hurt to leave it in.
	state->current_while_cond = NULL;
	state->current_while_end = NULL;
}

/*
Builds an if block from `node`.
*/
void gen_control_if(const AstNode **node, SkullStateLLVM *state) {
	gen_control_if_(
		node,
		LLVMGetInsertBlock(state->builder),
		LLVMAppendBasicBlockInContext(
			state->ctx,
			state->current_func->ref,
			"end"
		),
		state
	);
}

/*
Internal function for building an `if` node.
*/
static void gen_control_if_(
	const AstNode **node,
	LLVMBasicBlockRef entry,
	LLVMBasicBlockRef end,
	SkullStateLLVM *state
) {
	const AstNode *next_non_comment = (*node)->next;

	while (next_non_comment) {
		if (next_non_comment->type == AST_NODE_COMMENT) {
			next_non_comment = next_non_comment->next;
			continue;
		}

		break;
	}

	LLVMBasicBlockRef if_cond_true = LLVMAppendBasicBlockInContext(
		state->ctx,
		state->current_func->ref,
		"if_true"
	);
	LLVMBasicBlockRef if_cond_false = NULL;
	LLVMMoveBasicBlockBefore(if_cond_true, end);

	LLVMPositionBuilderAtEnd(state->builder, if_cond_true);
	gen_control_code_block(*node, end, state);
	LLVMPositionBuilderAtEnd(state->builder, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_cond_false = LLVMAppendBasicBlockInContext(
			state->ctx,
			state->current_func->ref,
			"if_false"
		);
		LLVMMoveBasicBlockAfter(end, if_cond_false);

		LLVMValueRef cond = gen_expr((*node)->expr, state).value;
		add_llvm_debug_info(
			cond,
			find_expr_node_location((*node)->expr),
			state
		);

		LLVMBuildCondBr(
			state->builder,
			cond,
			if_cond_true,
			if_cond_false
		);

		*node = next_non_comment;
	}

	// if there is an elif block following the current if block
	if (next_non_comment && next_non_comment->type == AST_NODE_ELIF) {
		gen_control_if_(node, if_cond_false, end, state);
	}
	// if there is an else block following the current if block
	else if (next_non_comment && next_non_comment->type == AST_NODE_ELSE) {
		LLVMPositionBuilderAtEnd(state->builder, if_cond_false);
		gen_control_code_block(*node, end, state);
	}
	// just a single if statement
	else {
		LLVMValueRef cond = gen_expr((*node)->expr, state).value;
		add_llvm_debug_info(
			cond,
			find_expr_node_location((*node)->expr),
			state
		);

		LLVMBuildCondBr(
			state->builder,
			cond,
			if_cond_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(state->builder, end);
}

Expr gen_stmt_break(const SkullStateLLVM *state) {
	return (Expr){
		.value = LLVMBuildBr(state->builder, state->current_while_end),
		.type = &TYPE_VOID
	};
}

Expr gen_stmt_continue(const SkullStateLLVM *state) {
	return (Expr){
		.value = LLVMBuildBr(state->builder, state->current_while_cond),
		.type = &TYPE_VOID
	};
}

/*
Parse `node` while in a new scope. Branch to `block` if no return occurred.
*/
static void gen_control_code_block(
	const AstNode *node,
	LLVMBasicBlockRef block,
	SkullStateLLVM *state
) {
	SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->child;

	LLVMMetadataRef old_di_scope = add_llvm_control_flow_debug_info(
		&node->token->location
	);

	const Expr returned = gen_tree(node->child, state);

	if (BUILD_DATA.debug) DEBUG_INFO.scope = old_di_scope;

	restore_parent_scope();

	if (SEMANTIC_STATE.scope) SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->next;

	if (!returned.value) LLVMBuildBr(state->builder, block);
}
