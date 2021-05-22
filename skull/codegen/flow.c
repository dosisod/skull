#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/assign.h"
#include "skull/codegen/expr.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/scope.h"
#include "skull/compiler/types/bool.h"
#include "skull/compiler/types/int.h"
#include "skull/compiler/variable.h"

#include "skull/codegen/flow.h"

Expr gen_node(AstNode *, bool *);
bool assert_sane_child(AstNode *);

/*
Build an unreachable statement.
*/
Expr gen_stmt_unreachable(void) {
	LLVMBuildUnreachable(SKULL_STATE.builder);

	return (Expr){ .type = TYPE_VOID };
}

/*
Builds an return statement from `node`.

Set `err` if error occurrs.
*/
Expr gen_stmt_return(AstNode **node, bool *err) {
	AstNode *const node_val = (*node)->next;
	if (!assert_sane_child(node_val)) {
		*err = true;
		return (Expr){0};
	}

	Expr expr = node_to_expr(NULL, node_val, err);

	const bool is_main = SKULL_STATE.current_func == SKULL_STATE.main_func;
	const bool is_void_return = (*node)->is_void_return;
	Type return_type = SKULL_STATE.current_func->return_type;

	if (*err || (is_void_return && return_type != TYPE_VOID)) {
		if (!*err) {
			FMT_ERROR(ERR_RETURN_MISSING_EXPR, {
				.loc = &node_val->token->location
			});
			*err = true;
		}
		return (Expr){0};
	}

	if (is_main && expr.type != TYPE_INT) {
		FMT_ERROR(ERR_NON_INT_MAIN, { .tok = node_val->token });

		*err = true;
		return (Expr){0};
	}

	if (return_type != TYPE_VOID && expr.type != return_type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &node_val->token->location, .type = return_type },
			{ .type = expr.type }
		);

		*err = true;
		return (Expr){0};
	}

	if (return_type == TYPE_VOID) {
		LLVMBuildRetVoid(SKULL_STATE.builder);
		expr.type = TYPE_VOID;
	}
	else LLVMBuildRet(SKULL_STATE.builder, expr.value);

	if (!is_void_return) *node = node_val;

	return expr;
}

static LLVMValueRef node_to_bool(const AstNode *const, bool *);

static bool gen_control_code_block(
	const char *,
	const AstNode *const,
	LLVMBasicBlockRef
);

/*
Builds LLVM for a while loop from `node`.
*/
bool gen_control_while(AstNode **node) {
	LLVMBasicBlockRef while_cond = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func->ref,
		"while_cond"
	);
	LLVMBasicBlockRef while_loop = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func->ref,
		"while_loop"
	);
	LLVMBasicBlockRef while_end = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		SKULL_STATE.current_func->ref,
		"while_end"
	);

	LLVMBuildBr(SKULL_STATE.builder, while_cond);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_cond);

	*node = (*node)->next;

	bool err = false;
	LLVMValueRef cond = node_to_bool(*node, &err);
	if (err) return true;

	LLVMBuildCondBr(
		SKULL_STATE.builder,
		cond,
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_loop);

	if (gen_control_code_block("while", *node, while_cond))
		return true;

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, while_end);

	return false;
}

bool gen_control_if_(AstNode **, LLVMBasicBlockRef, LLVMBasicBlockRef);

/*
Builds an if block from `node`.
*/
bool gen_control_if(AstNode **node) {
	return gen_control_if_(
		node,
		LLVMGetInsertBlock(SKULL_STATE.builder),
		LLVMAppendBasicBlockInContext(
			SKULL_STATE.ctx,
			SKULL_STATE.current_func->ref,
			"end"
		)
	);
}

/*
Internal function for building an `if` node.
*/
bool gen_control_if_(
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
		SKULL_STATE.current_func->ref,
		"if_true"
	);
	LLVMBasicBlockRef if_false = NULL;
	LLVMMoveBasicBlockBefore(if_true, end);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, if_true);
	*node = (*node)->next;
	if (gen_control_code_block("if", *node, end))
		return true;

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_false = LLVMAppendBasicBlockInContext(
			SKULL_STATE.ctx,
			SKULL_STATE.current_func->ref,
			"if_false"
		);
		LLVMMoveBasicBlockAfter(end, if_false);

		bool err = false;
		LLVMValueRef cond = node_to_bool(*node, &err);
		if (err) return true;

		LLVMBuildCondBr(
			SKULL_STATE.builder,
			cond,
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

		if (gen_control_code_block("else", *node, end))
			return true;
	}
	// just a single if statement
	else {
		bool err = false;
		LLVMValueRef cond = node_to_bool(*node, &err);
		if (err) return true;

		LLVMBuildCondBr(
			SKULL_STATE.builder,
			cond,
			if_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, end);

	return false;
}

/*
Try and parse a condition (something returning a bool) from `node`.

Set `err` if an error occurred.
*/
static LLVMValueRef node_to_bool(const AstNode *const node, bool *err) {
	const Expr expr = node_to_expr(NULL, node, err);
	if (*err) return NULL;

	if (expr.type != TYPE_BOOL) {
		FMT_ERROR(ERR_NON_BOOL_EXPR, { .loc = &node->token->location });

		*err = true;
		return NULL;
	}

	if (LLVMIsConstant(expr.value))
		FMT_WARN(LLVMConstIntGetSExtValue(expr.value) ?
			WARN_COND_ALWAYS_TRUE :
			WARN_COND_ALWAYS_FALSE,
			{ .loc = &node->token->location }
		);

	return expr.value;
}

/*
Parse `node` while in a new scope. Branch to `block` when done.

`name` is the type of block: if, else, while, etc.

Return `true` if error occurred.
*/
static bool gen_control_code_block(
	const char *name,
	const AstNode *const node,
	LLVMBasicBlockRef block
) {
	if (!node->child) { // NOLINT
		FMT_ERROR(ERR_MISSING_BLOCK, {
			.loc = &node->token->location,
			.real = strdup(name)
		});

		return true;
	}

	Scope *scope_copy;
	make_sub_scope(&SKULL_STATE.scope, &scope_copy);

	if (node->child->token) {
		bool err = false;
		const Expr returned = gen_node(node->child, &err);
		if (err) return true;

		if (!returned.value)
			LLVMBuildBr(SKULL_STATE.builder, block);
	}
	else {
		LLVMBuildBr(SKULL_STATE.builder, block);
	}

	restore_sub_scope(&SKULL_STATE.scope, &scope_copy);

	return false;
}
