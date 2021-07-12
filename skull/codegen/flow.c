#include <stdbool.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/common/errors.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/codegen/flow.h"

Expr gen_node(AstNode *, bool *);
static void warn_const_cond(LLVMValueRef, Location *);
static LLVMValueRef node_to_bool(const AstNode *const);
static bool gen_control_if_(AstNode **, LLVMBasicBlockRef, LLVMBasicBlockRef);
static LLVMMetadataRef add_llvm_control_flow_debug_info(const Location *);

static bool gen_control_code_block(
	const char *,
	const AstNode *const,
	LLVMBasicBlockRef
);


/*
Build an unreachable statement.
*/
Expr gen_stmt_unreachable(void) {
	LLVMBuildUnreachable(SKULL_STATE_LLVM.builder);

	return (Expr){ .type = TYPE_VOID };
}

void gen_stmt_implicit_main_return(Location *location) {
	LLVMValueRef ret = LLVMBuildRet(SKULL_STATE_LLVM.builder, LLVM_INT(0));

	add_llvm_debug_info(ret, location);
}

/*
Builds an return statement from `node`.

Set `err` if error occurrs.
*/
Expr gen_stmt_return(AstNode *node, bool *err) {
	AstNode *const expr_node = node->expr_node;
	Type return_type = SKULL_STATE_LLVM.current_func->return_type;

	Expr expr = node_to_expr(NULL, expr_node, err);

	if (*err || (!expr_node && return_type != TYPE_VOID)) {
		if (!*err) {
			FMT_ERROR(ERR_RETURN_MISSING_EXPR, {
				.loc = &node->token->location
			});
			*err = true;
		}
		return (Expr){0};
	}

	const bool is_main = \
		SKULL_STATE_LLVM.current_func == SKULL_STATE_LLVM.main_func;

	if (is_main && expr.type != TYPE_INT) {
		FMT_ERROR(ERR_NON_INT_MAIN, { .tok = expr_node->token }); // NOLINT

		*err = true;
		return (Expr){0};
	}

	if (return_type != TYPE_VOID && expr.type != return_type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &expr_node->token->location, .type = return_type },
			{ .type = expr.type }
		);

		*err = true;
		return (Expr){0};
	}

	if (return_type == TYPE_VOID) expr.type = TYPE_VOID;
	LLVMValueRef ret = LLVMBuildRet(SKULL_STATE_LLVM.builder, expr.value);

	add_llvm_debug_info(ret, &node->token->location);

	return expr;
}

/*
Builds LLVM for a while loop from `node`.

Return `true` if an error occurred.
*/
bool gen_control_while(AstNode *node) {
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

	LLVMBuildBr(SKULL_STATE_LLVM.builder, while_cond);
	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_cond);

	bool err = false;
	const Expr expr = node_to_expr(NULL, node->expr_node, &err);
	if (err) return NULL;

	warn_const_cond(expr.value, &node->expr_node->token->location);

	LLVMBuildCondBr(
		SKULL_STATE_LLVM.builder,
		expr.value,
		while_loop,
		while_end
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_loop);

	if (gen_control_code_block("while", node, while_cond))
		return true;

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, while_end);

	return false;
}

/*
Builds an if block from `node`.
*/
bool gen_control_if(AstNode **node) {
	return gen_control_if_(
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

Return `true` if an error occurred.
*/
static bool gen_control_if_(
	AstNode **node,
	LLVMBasicBlockRef entry,
	LLVMBasicBlockRef end
) {
	AstNode *next_non_comment = (*node)->next;

	while (next_non_comment) {
		if (next_non_comment->type == AST_NODE_COMMENT) {
			next_non_comment = next_non_comment->next;
			continue;
		}

		break;
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlockInContext(
		SKULL_STATE_LLVM.ctx,
		SKULL_STATE_LLVM.current_func->ref,
		"if_true"
	);
	LLVMBasicBlockRef if_false = NULL;
	LLVMMoveBasicBlockBefore(if_true, end);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, if_true);

	if (gen_control_code_block("if", *node, end)) return true;

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, entry);

	if (next_non_comment && (
		next_non_comment->type == AST_NODE_ELIF ||
		next_non_comment->type == AST_NODE_ELSE)
	) {
		if_false = LLVMAppendBasicBlockInContext(
			SKULL_STATE_LLVM.ctx,
			SKULL_STATE_LLVM.current_func->ref,
			"if_false"
		);
		LLVMMoveBasicBlockAfter(end, if_false);

		LLVMValueRef cond = node_to_bool((*node)->expr_node);
		if (!cond) return true;

		LLVMBuildCondBr(
			SKULL_STATE_LLVM.builder,
			cond,
			if_true,
			if_false
		);

		*node = next_non_comment;
	}

	// if there is an elif block following the current if block
	if (next_non_comment && next_non_comment->type == AST_NODE_ELIF) {
		gen_control_if_(node, if_false, end);
	}
	// if there is an else block following the current if block
	else if (next_non_comment && next_non_comment->type == AST_NODE_ELSE) {
		LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, if_false);

		if (gen_control_code_block("else", *node, end)) return true;
	}
	// just a single if statement
	else {
		LLVMValueRef cond = node_to_bool((*node)->expr_node);
		if (!cond) return true;

		LLVMBuildCondBr(
			SKULL_STATE_LLVM.builder,
			cond,
			if_true,
			end
		);
	}

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, end);

	return false;
}

/*
Try and parse a condition (something returning a bool) from `node`.

Return `NULL` if an error occurred.
*/
static LLVMValueRef node_to_bool(const AstNode *const node) {
	bool err = false;
	const Expr expr = node_to_expr(NULL, node, &err);
	if (err) return NULL;

	if (expr.type != TYPE_BOOL) {
		FMT_ERROR(ERR_NON_BOOL_EXPR, { .loc = &node->token->location });

		return NULL;
	}

	warn_const_cond(expr.value, &node->token->location);

	return expr.value;
}


static void warn_const_cond(LLVMValueRef value, Location *location) {
	if (LLVMIsConstant(value))
		FMT_WARN(LLVMConstIntGetSExtValue(value) ?
			WARN_COND_ALWAYS_TRUE :
			WARN_COND_ALWAYS_FALSE,
			{ .loc = location }
		);
}

/*
Parse `node` while in a new scope. Branch to `block` if no return occurred.

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
			.str = name
		});

		return true;
	}

	SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->child;

	LLVMMetadataRef old_di_scope = add_llvm_control_flow_debug_info(
		&node->token->location
	);

	Expr returned = (Expr){0};

	bool err = false;
	if (node->child->token) returned = gen_node(node->child, &err);

	if (BUILD_DATA.debug) DEBUG_INFO.scope = old_di_scope;

	restore_parent_scope();

	if (err) return true;
	if (SEMANTIC_STATE.scope) SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->next;

	if (!returned.value || !node->child->token)
		LLVMBuildBr(SKULL_STATE_LLVM.builder, block);

	return false;
}

static LLVMMetadataRef add_llvm_control_flow_debug_info(
	const Location *location
) {
	if (!BUILD_DATA.debug) return NULL;

	LLVMMetadataRef old_di_scope = DEBUG_INFO.scope;

	DEBUG_INFO.scope = LLVMDIBuilderCreateLexicalBlock(
		DEBUG_INFO.builder,
		DEBUG_INFO.scope,
		DEBUG_INFO.file,
		location->line,
		location->column
	);

	return old_di_scope;
}
