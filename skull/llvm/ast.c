#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"
#include "skull/llvm/aliases.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"

#include "skull/llvm/ast.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

// module-level variables only accessible from functions in this file
static Scope *scope;
static LLVMValueRef func;
static LLVMModuleRef module;
static LLVMBuilderRef builder;

#define EXTERNAL_FUNCTIONS_MAX 256
unsigned external_functions = 0;
char *external_function[EXTERNAL_FUNCTIONS_MAX] = {0};

void llvm_assign_identifier(Variable *const var, const AstNode *const node);

/*
Convert skull code from `str_` into LLVM IR (using `func_` and `module_`).
*/
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMModuleRef module_, LLVMBuilderRef _builder) {
	char32_t *const str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	char32_t *error = NULL;
	AstNode *const node = make_ast_tree(str, &error);

	if (!node) {
		PANIC(error);
	}

	scope = make_scope();
	func = func_;
	module = module_;
	builder = _builder;

	node_to_llvm_ir(node);
	free_ast_tree(node);
	free(str);

	char **current_function = external_function;
	while (*current_function) {
		free(*current_function);
		current_function++;
	}
}

/*
Internal LLVM IR parser.
*/
void node_to_llvm_ir(AstNode *node) {
	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(node);
		}

		else if (node->node_type == AST_NODE_VAR_DEF) {
			llvm_make_var_def(&node);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(node);
		}

		else if (node->node_type == AST_NODE_EXTERNAL) {
			declare_external_function(node);
		}

		else if (node->node_type == AST_NODE_FUNCTION) {
			llvm_make_function(node);
		}

		else if (node->node_type == AST_NODE_VAR_ASSIGN) {
			llvm_make_assign(&node);
		}

		else {
			PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
		}

		node = node->next;
	}
}

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode *node) {
	if (node->node_type != AST_NODE_RETURN) {
		PANIC(U"Return expected");
	}

	if (node->token->next->token_type == TOKEN_IDENTIFIER) {
		char32_t *const var_name = token_str(node->token->next);
		const Variable *const found_var = scope_find_name(scope, var_name);

		if (!found_var) {
			PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
		}
		free(var_name);

		if (found_var->type != &TYPE_INT) {
			PANIC(FMT_ERROR(ERR_NON_INT_RETURN, { .var = found_var }));
		}

		LLVMBuildRet(
			builder,
			LLVMBuildLoad2(
				builder,
				LLVMInt64Type(),
				found_var->alloca,
				""
			)
		);
	}
	else {
		char32_t *error = NULL;

		LLVMBuildRet(
			builder,
			LLVM_INT(eval_integer(node->token->next, &error))
		);

		PANIC_ON_ERR(error);
	}
}

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	node_make_var(*node, scope);

	llvm_make_assign_(scope->vars[scope->vars_used - 1], (*node)->next);

	*node = (*node)->next;
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
		char32_t *const var_name = token_str(node->token->next);
		const Variable *const found_var = scope_find_name(scope, var_name);

		if (!found_var) {
			PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
		}
		free(var_name);

		if (found_var->type != &TYPE_BOOL) {
			PANIC(FMT_ERROR(U"Expected \"%\" to be of type bool", { .var = found_var }));
		}

		cond = LLVMBuildLoad2(
			builder,
			LLVMInt1Type(),
			found_var->alloca,
			""
		);
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(func, "if_true");
	LLVMBasicBlockRef end = LLVMAppendBasicBlock(func, "end");

	LLVMBuildCondBr(
		builder,
		cond,
		if_true,
		end
	);

	LLVMPositionBuilderAtEnd(
		builder,
		if_true
	);

	if (node->child->token) {
		node_to_llvm_ir(node->child);
	}

	LLVMBuildBr(builder, end);

	LLVMPositionBuilderAtEnd(
		builder,
		end
	);
}

typedef LLVMValueRef (*MathOper)(Variable *, const Token *, const Token *);

/*
Build LLVM for assigning math operation `oper` from `node` to `var`.
*/
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper oper, const char32_t *panic) {
	const Token *lhs = node->token;
	const Token *rhs = node->token->next->next;

	LLVMValueRef result = NULL;

	if (lhs->token_type == rhs->token_type) {
		result = oper(var, lhs, rhs);
	}

	if (!result) {
		PANIC(FMT_ERROR(
			panic,
			{ .tok = lhs },
			{ .tok = rhs }
		));
	}

	LLVMBuildStore(
		builder,
		result,
		var->alloca
	);
}

/*
Build LLVM for assining addition of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_add(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef add;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		add = LLVMBuildAdd(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		add = LLVMBuildFAdd(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return add;
}

/*
Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_sub(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef sub;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		sub = LLVMBuildSub(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		sub = LLVMBuildFSub(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return sub;
}

/*
Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_mult(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef mult;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		mult = LLVMBuildMul(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		mult = LLVMBuildFMul(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return mult;
}

/*
Build LLVM for assining division of `lhs` and `rhs` to `var`.
*/
LLVMValueRef llvm_make_div(Variable *var, const Token *lhs, const Token *rhs) {
	char32_t *error = NULL;
	LLVMValueRef div;

	if (lhs->token_type == TOKEN_INT_CONST && var->type == &TYPE_INT) {
		div = LLVMBuildSDiv(
			builder,
			LLVM_INT(eval_integer(lhs, &error)),
			LLVM_INT(eval_integer(rhs, &error)),
			""
		);
	}

	else if (lhs->token_type == TOKEN_FLOAT_CONST && var->type == &TYPE_FLOAT) {
		div = LLVMBuildFDiv(
			builder,
			LLVM_FLOAT(eval_float(lhs, &error)),
			LLVM_FLOAT(eval_float(rhs, &error)),
			""
		);
	}

	else {
		return NULL;
	}

	PANIC_ON_ERR(error);
	return div;
}

/*
Store function name of externaly declared function in `node`.
*/
void declare_external_function(AstNode *node) {
	char32_t *const wide_func_name = token_str(node->token->next);
	char *const func_name = c32stombs(wide_func_name);
	free(wide_func_name);

	external_function[external_functions] = func_name;
	external_functions++;
}

/*
Builds a function declaration from `node`.
*/
void llvm_make_function(AstNode *node) {
	LLVMTypeRef args[] = { LLVMVoidType() };

	LLVMTypeRef type = LLVMFunctionType(
		LLVMVoidType(),
		args,
		0,
		false
	);

	char32_t *const wide_func_name = token_str(node->token);
	char *const func_name = c32stombs(wide_func_name);

	char **current_function = external_function;
	while (*current_function) {
		if (strcmp(*current_function, func_name) == 0) {
			break;
		}
		current_function++;
	}

	if (!*current_function) {
		PANIC(FMT_ERROR(U"external function \"%\" missing external declaration", { .real = wide_func_name }));
	}

	free(wide_func_name);

	LLVMValueRef function = LLVMAddFunction(
		module,
		func_name,
		type
	);

	free(func_name);

	LLVMSetLinkage(function, LLVMExternalLinkage);

	LLVMBuildCall2(
		builder,
		type,
		function,
		NULL,
		0,
		""
	);
}

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_assign(AstNode **node) {
	char32_t *const var_name = token_str((*node)->token);
	Variable *const found_var = scope_find_name(scope, var_name);

	if (!found_var) {
		PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
	}

	if (found_var->is_const) {
		PANIC(FMT_ERROR(ERR_CANNOT_ASSIGN_CONST, { .real = var_name }));
	}
	free(var_name);

	llvm_make_assign_(found_var, (*node)->next);

	*node = (*node)->next;
}

/*
Internal function to build LLVM assignment from `node` to `var.
*/
void llvm_make_assign_(Variable *const var, const AstNode *const node) {
	if (!node) {
		PANIC(FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .var = var }));
	}

	if (node->node_type == AST_NODE_IDENTIFIER) {
		llvm_assign_identifier(var, node);
		return;
	}

	char *const var_name = c32stombs(var->name);

	if (!var->alloca) {
		var->alloca = LLVMBuildAlloca(
			builder,
			var->type->llvm_type(),
			var_name
		);
	}

	if (node->node_type == AST_NODE_ADD_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_add, U"cannot add \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_SUB_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_sub, U"cannot subtract \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_MULT_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_mult, U"cannot multiply \"%\" and \"%\"");
		free(var_name);
		return;
	}

	if (node->node_type == AST_NODE_DIV_CONSTS) {
		llvm_make_math_oper(var, node, &llvm_make_div, U"cannot divide \"%\" and \"%\"");
		free(var_name);
		return;
	}

	char32_t *err = NULL;

	if (var->type == &TYPE_INT && node->node_type == AST_NODE_INT_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_INT(eval_integer(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_FLOAT && node->node_type == AST_NODE_FLOAT_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_FLOAT(eval_float(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_BOOL && node->node_type == AST_NODE_BOOL_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_BOOL(eval_bool(node->token)),
			var->alloca
		);
	}
	else if (var->type == &TYPE_RUNE && node->node_type == AST_NODE_RUNE_CONST) {
		LLVMBuildStore(
			builder,
			LLVM_RUNE(eval_rune(node->token, &err)),
			var->alloca
		);

		PANIC_ON_ERR(err);
	}
	else if (var->type == &TYPE_STR && node->node_type == AST_NODE_STR_CONST) {
		SkullStr str = eval_str(node->token, &err);

		char *const mbs = c32stombs(str);
		const unsigned len = (unsigned)strlen(mbs);

		LLVMBuildStore(
			builder,
			LLVMConstString(mbs, len, false),
			LLVMBuildAlloca(
				builder,
				LLVMArrayType(
					LLVMInt8Type(),
					len + 1
				),
				""
			)
		);
		free(mbs);

		LLVMTypeRef str_ptr = LLVMPointerType(LLVMInt8Type(), 0);

		LLVMBuildStore(
			builder,
			LLVMBuildBitCast(
				builder,
				var->alloca,
				str_ptr,
				""
			),
			var->alloca
		);
	}
	else {
		PANIC(FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type }));
	}

	free(var_name);
}

/*
Build LLVM to assign an existing identifier `node` to `var`.
*/
void llvm_assign_identifier(Variable *const var, const AstNode *const node) {
	char32_t *const lookup = token_str(node->token);
	const Variable *const var_found = scope_find_name(scope, lookup);

	if (!var_found) {
		PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup }));
	}
	free(lookup);
	if (var_found->type != var->type) {
		PANIC(FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type }));
	}

	if (!var->alloca) {
		var->alloca = LLVMBuildAlloca(
			builder,
			var->type->llvm_type(),
			c32stombs(var->name)
		);
	}

	LLVMValueRef load = LLVMBuildLoad2(
		builder,
		var->type->llvm_type(),
		var_found->alloca,
		""
	);

	LLVMBuildStore(
		builder,
		load,
		var->alloca
	);
}