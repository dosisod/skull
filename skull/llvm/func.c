#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/scope.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/func.h"

extern LLVMBuilderRef BUILDER;
extern LLVMModuleRef MODULE;
extern LLVMValueRef CURRENT_FUNC;
extern Scope *SCOPE;

FunctionDeclaration *FUNCTION_DECLARATIONS = NULL;

bool node_to_llvm_ir(AstNode *);

/*
Parse declaration (and potential definition) of function in `node`.
*/
void declare_function(AstNode *node) {
	char32_t *wide_func_name = NULL;
	const bool is_external = ATTR(AstNodeFunctionProto, node, is_external);

	if (is_external) {
		wide_func_name = token_str(node->token->next);
	}
	else {
		wide_func_name = token_str(node->token);
	}

	char *const func_name = c32stombs(wide_func_name);

	FunctionDeclaration *f;
	f = calloc(1, sizeof *f);
	DIE_IF_MALLOC_FAILS(f);

	f->name = func_name;

	f->param_types = ATTR(AstNodeFunctionProto, node, param_types);
	f->param_names = ATTR(AstNodeFunctionProto, node, param_names);
	LLVMTypeRef *params = NULL;

	if (f->param_types) {
		f->num_params = 1;

		LLVMTypeRef llvm_param_type = f->param_types->llvm_type();
		params = &llvm_param_type;
	}

	f->return_type = ATTR(AstNodeFunctionProto, node, return_type);
	LLVMTypeRef llvm_return_type = LLVMVoidType();

	if (f->return_type) {
		llvm_return_type = f->return_type->llvm_type();
	}

	f->type = LLVMFunctionType(
		llvm_return_type,
		params,
		f->num_params,
		false
	);

	f->function = LLVMAddFunction(
		MODULE,
		func_name,
		f->type
	);
	LLVMSetLinkage(f->function, LLVMExternalLinkage);

	FunctionDeclaration *head = FUNCTION_DECLARATIONS;
	while (head) {
		if (strcmp(func_name, head->name) == 0) {
			PANIC("cannot redeclare function \"%s\"\n", { .str = wide_func_name });
		}

		if (!head->next) {
			break;
		}
		head = head->next;
	}
	free(wide_func_name);

	if (head) {
		head->next = f;
	}
	else {
		FUNCTION_DECLARATIONS = f;
	}

	if (!is_external) {
		define_function(node);
	}
}

/*
Builds a function call from `node`.
*/
LLVMValueRef llvm_make_function_call(const AstNode *const node) {
	char32_t *const wide_func_name = token_str(node->token);
	char *const func_name = c32stombs(wide_func_name);

	FunctionDeclaration *current_function = FUNCTION_DECLARATIONS;
	while (current_function) {
		if (strcmp(current_function->name, func_name) == 0) {
			break;
		}
		current_function = current_function->next;
	}
	free(func_name);

	if (!current_function) {
		PANIC(ERR_MISSING_DECLARATION, { .str = wide_func_name });
	}
	free(wide_func_name);

	LLVMValueRef params = NULL;

	if (current_function->num_params == 1) {
		Token *param = ATTR(AstNodeFunction, node, param);

		if (param->token_type == TOKEN_IDENTIFIER) {
			SCOPE_FIND_VAR(var_found, param, lookup);
			free(lookup);

			if (var_found->type != current_function->param_types) {
				PANIC(ERR_TYPE_MISMATCH, {
					.real = strdup(current_function->param_types->name)
				});
			}

			params = llvm_var_get_value(var_found);
		}
		else if (current_function->param_types == token_type_to_type(param)) {
			params = llvm_parse_token(param);
		}
		else {
			PANIC(ERR_FUNC_TYPE_MISMATCH, {
				.real = strdup(current_function->param_types->name)
			});
		}
	}

	return LLVMBuildCall2(
		BUILDER,
		current_function->type,
		current_function->function,
		&params,
		current_function->num_params,
		""
	);
}

/*
Create a native LLVM function.
*/
void define_function(const AstNode *const node) {
	char32_t *const wide_func_name = token_str(node->token);
	char *const func_name = c32stombs(wide_func_name);

	FunctionDeclaration *current_function = FUNCTION_DECLARATIONS;
	while (current_function) {
		if (strcmp(current_function->name, func_name) == 0) {
			break;
		}
		current_function = current_function->next;
	}
	free(func_name);

	if (!current_function) {
		PANIC(ERR_MISSING_DECLARATION, { .str = wide_func_name });
	}
	free(wide_func_name);

	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(CURRENT_FUNC);

	LLVMValueRef old_func = CURRENT_FUNC;
	CURRENT_FUNC = current_function->function;

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(
		current_function->function,
		"entry"
	);

	LLVMPositionBuilderAtEnd(BUILDER, entry);

	MAKE_SUB_SCOPE;

	if (current_function->param_types) {
		Variable *param_var = make_variable(
			current_function->param_types,
			current_function->param_names,
			true
		);

		if (!scope_add_var(SCOPE, param_var)) {
			PANIC("variable \"%s\" shadows existing variable\n", { .var = param_var });
		}

		param_var->alloca = LLVMGetFirstParam(current_function->function);
	}

	bool returned = node_to_llvm_ir(node->child);

	if (!returned && current_function->return_type) {
		PANIC("expected return value in function \"%s\"\n",
			{ .real = current_function->name }
		);
	}
	if (returned && !current_function->return_type) {
		PANIC("unexpected return from void function \"%s\"\n",
			{ .real = current_function->name }
		);
	}

	RESTORE_SUB_SCOPE;

	if (!current_function->return_type) {
		LLVMBuildRetVoid(BUILDER);
	}

	LLVMPositionBuilderAtEnd(BUILDER, current_block);

	CURRENT_FUNC = old_func;
}
