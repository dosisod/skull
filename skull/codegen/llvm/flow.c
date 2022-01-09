#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/codegen/llvm/flow.h"

Expr gen_tree(const AstNode *);
static void gen_control_if_(
	const AstNode **,
	LLVMBasicBlockRef,
	LLVMBasicBlockRef
);

static void gen_control_code_block(const AstNode *, LLVMBasicBlockRef);


/*
Build an unreachable statement.
*/
Expr gen_stmt_unreachable(void) {
	return (Expr){
		.value = LLVMBuildUnreachable(SKULL_STATE_LLVM.builder),
		.type = &TYPE_VOID
	};
}

void gen_stmt_implicit_main_return(const Location *location) {
	LLVMValueRef ret = LLVMBuildRet(SKULL_STATE_LLVM.builder, LLVM_INT(0));

	add_llvm_debug_info(ret, location);
}

void gen_stmt_noop(const Location *location) {
	if (!BUILD_DATA.debug) return;

	LLVMValueRef noop = LLVMBuildStore(
		SKULL_STATE_LLVM.builder,
		LLVM_INT(0),
		LLVMBuildAlloca(
			SKULL_STATE_LLVM.builder,
			type_to_llvm_type(&TYPE_INT),
			"noop"
		)
	);

	add_llvm_debug_info(noop, location);
}

/*
Builds an return statement from `node`.
*/
Expr gen_stmt_return(const AstNode *node) {
	if (!node->expr) {
		return (Expr){
			.value = LLVMBuildRetVoid(SKULL_STATE_LLVM.builder),
			.type = &TYPE_VOID
		};
	}

	const Expr expr = gen_expr(node->expr);

	LLVMValueRef ret = LLVMBuildRet(SKULL_STATE_LLVM.builder, expr.value);

	add_llvm_debug_info(ret, &node->token->location);

	return expr;
}

/*
Builds LLVM for a while loop from `node`.
*/
void gen_control_while(const AstNode *node) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlockInContext(
		SKULL_STATE_LLVM.ctx,
		SKULL_STATE_LLVM.current_func->ref,
		"while_cond"
	);
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlockInContext(
		SKULL_STATE_LLVM.ctx,
		SKULL_STATE_LLVM.current_func->ref,
		"while_loop"
	);
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlockInContext(
		SKULL_STATE_LLVM.ctx,
		SKULL_STATE_LLVM.current_func->ref,
		"while_end"
	);

	SKULL_STATE_LLVM.current_while_cond = while_cond;
	SKULL_STATE_LLVM.current_while_end = while_end;

	LLVMBuildBr(SKULL_STATE_LLVM.builder, while_cond);
	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_cond);

	LLVMValueRef cond = gen_expr(node->expr).value;
	add_llvm_debug_info(cond, find_expr_node_location(node->expr));

	LLVMBuildCondBr(
		SKULL_STATE_LLVM.builder,
		cond,
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_loop);
	gen_control_code_block(node, while_cond);
	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_end);

	// shouldn't be needed since semantic layer checks that all break/continue
	// statements are in a while loop, but it doesn't hurt to leave it in.
	SKULL_STATE_LLVM.current_while_cond = NULL;
	SKULL_STATE_LLVM.current_while_end = NULL;
}

/*
Builds an if block from `node`.
*/
void gen_control_if(const AstNode **node) {
	gen_control_if_(
		node,
		LLVMGetInsertBlock(SKULL_STATE_LLVM.builder),
		LLVMAppendBasicBlockInContext(
			SKULL_STATE_LLVM.ctx,
			SKULL_STATE_LLVM.current_func->ref,
			"end"
		)
	);
}

/*
Internal function for building an `if` node.
*/
static void gen_control_if_(
	const AstNode **node,
	LLVMBasicBlockRef entry,
	LLVMBasicBlockRef end
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
		SKULL_STATE_LLVM.ctx,
		SKULL_STATE_LLVM.current_func->ref,
		"if_true"
	);
	LLVMBasicBlockRef if_cond_false = NULL;
	LLVMMoveBasicBlockBefore(if_cond_true, end);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, if_cond_true);
	gen_control_code_block(*node, end);
	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_cond_false = LLVMAppendBasicBlockInContext(
			SKULL_STATE_LLVM.ctx,
			SKULL_STATE_LLVM.current_func->ref,
			"if_false"
		);
		LLVMMoveBasicBlockAfter(end, if_cond_false);

		LLVMValueRef cond = gen_expr((*node)->expr).value;
		add_llvm_debug_info(
			cond,
			find_expr_node_location((*node)->expr)
		);

		LLVMBuildCondBr(
			SKULL_STATE_LLVM.builder,
			cond,
			if_cond_true,
			if_cond_false
		);

		*node = next_non_comment;
	}

	// if there is an elif block following the current if block
	if (next_non_comment && next_non_comment->type == AST_NODE_ELIF) {
		gen_control_if_(node, if_cond_false, end);
	}
	// if there is an else block following the current if block
	else if (next_non_comment && next_non_comment->type == AST_NODE_ELSE) {
		LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, if_cond_false);
		gen_control_code_block(*node, end);
	}
	// just a single if statement
	else {
		LLVMValueRef cond = gen_expr((*node)->expr).value;
		add_llvm_debug_info(
			cond,
			find_expr_node_location((*node)->expr)
		);

		LLVMBuildCondBr(
			SKULL_STATE_LLVM.builder,
			cond,
			if_cond_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, end);
}

Expr gen_stmt_break(void) {
	return (Expr){
		.value = LLVMBuildBr(
			SKULL_STATE_LLVM.builder,
			SKULL_STATE_LLVM.current_while_end
		),
		.type = &TYPE_VOID
	};
}

Expr gen_stmt_continue(void) {
	return (Expr){
		.value = LLVMBuildBr(
			SKULL_STATE_LLVM.builder,
			SKULL_STATE_LLVM.current_while_cond
		),
		.type = &TYPE_VOID
	};
}

/*
Parse `node` while in a new scope. Branch to `block` if no return occurred.
*/
static void gen_control_code_block(
	const AstNode *node,
	LLVMBasicBlockRef block
) {
	SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->child;

	LLVMMetadataRef old_di_scope = add_llvm_control_flow_debug_info(
		&node->token->location
	);

	const Expr returned = gen_tree(node->child);

	if (BUILD_DATA.debug) DEBUG_INFO.scope = old_di_scope;

	restore_parent_scope();

	if (SEMANTIC_STATE.scope) SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->next;

	if (!returned.value) LLVMBuildBr(SKULL_STATE_LLVM.builder, block);
}
