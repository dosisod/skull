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

#include "skull/llvm/func.h"

extern LLVMBuilderRef BUILDER;
extern LLVMModuleRef MODULE;
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern Scope *SCOPE;

FunctionDeclaration *FUNCTION_DECLARATIONS = NULL;

bool node_to_llvm_ir(AstNode *);
FunctionDeclaration *llvm_create_new_function(AstNode *, char *, bool);

/*
Parse declaration (and potential definition) of function in `node`.
*/
void declare_function(AstNode *node) {
	const bool is_external = ATTR(AstNodeFunctionProto, node, is_external);
	const bool is_export = ATTR(AstNodeFunctionProto, node, is_export);

	Token *func_name_token = NULL;

	if (is_external || is_export) {
		func_name_token = node->token->next;
	}
	else {
		func_name_token = node->token;
	}
	char *func_name = token_mbs_str(func_name_token);

	if (is_export && CURRENT_FUNC != MAIN_FUNC) {
		PANIC(ERR_NO_EXPORT_NESTED, {
			.tok = func_name_token
		});
	}

	if (strcmp(func_name, "main") == 0) {
		PANIC(ERR_MAIN_RESERVED, {0});
	}

	FunctionDeclaration *head = FUNCTION_DECLARATIONS;
	while (head) {
		if (strcmp(func_name, head->name) == 0) {
			PANIC(ERR_NO_REDEFINE_FUNC, {
				.tok = func_name_token,
				.real = func_name
			});
		}

		if (!head->next) {
			break;
		}
		head = head->next;
	}

	FunctionDeclaration *func = llvm_create_new_function(
		node,
		func_name,
		is_export || is_external
	);

	if (head) {
		head->next = func;
	}
	else {
		FUNCTION_DECLARATIONS = func;
	}

	if (!is_external) {
		define_function(node, func);
	}
}

FunctionDeclaration *llvm_create_new_function(AstNode *node, char *name, bool is_private) {
	FunctionDeclaration *func;
	func = calloc(1, sizeof *func);
	DIE_IF_MALLOC_FAILS(func);

	func->name = name;

	func->param_types = ATTR(AstNodeFunctionProto, node, param_types);
	func->param_names = ATTR(AstNodeFunctionProto, node, param_names);
	LLVMTypeRef *params = NULL;

	if (func->param_types) {
		func->num_params = 1;

		LLVMTypeRef llvm_param_type = func->param_types->llvm_type();
		params = &llvm_param_type;
	}

	func->return_type = ATTR(AstNodeFunctionProto, node, return_type);
	LLVMTypeRef llvm_return_type = LLVMVoidType();

	if (func->return_type) {
		llvm_return_type = func->return_type->llvm_type();
	}

	func->type = LLVMFunctionType(
		llvm_return_type,
		params,
		func->num_params,
		false
	);

	func->function = LLVMAddFunction(
		MODULE,
		name,
		func->type
	);

	LLVMSetLinkage(
		func->function,
		is_private ?
			LLVMExternalLinkage :
			LLVMPrivateLinkage
	);

	return func;
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
		PANIC(ERR_MISSING_DECLARATION, {
			.tok = node->token,
			.str = wide_func_name
		});
	}
	free(wide_func_name);

	LLVMValueRef params = NULL;
	Token *param = ATTR(AstNodeFunction, node, param);

	if (current_function->num_params == 1) {
		if (param->token_type == TOKEN_IDENTIFIER) {
			SCOPE_FIND_VAR(var_found, param, lookup);
			free(lookup);

			if (var_found->type != current_function->param_types) {
				PANIC(ERR_TYPE_MISMATCH, {
					.tok = param,
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
				.tok = param,
				.real = strdup(current_function->param_types->name)
			});
		}
	}
	else if (param) {
		PANIC(ERR_ZERO_PARAM_FUNC, { .tok = param });
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
void define_function(const AstNode *const node, FunctionDeclaration *func) {
	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(CURRENT_FUNC);

	LLVMValueRef old_func = CURRENT_FUNC;
	CURRENT_FUNC = func->function;

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(
		func->function,
		"entry"
	);

	LLVMPositionBuilderAtEnd(BUILDER, entry);

	MAKE_SUB_SCOPE;

	if (func->param_types) {
		Variable *param_var = make_variable(
			func->param_types,
			func->param_names,
			true
		);

		if (!scope_add_var(SCOPE, param_var)) {
			PANIC(ERR_SHADOW_VAR, { .var = param_var });
		}

		param_var->alloca = LLVMGetFirstParam(func->function);
	}

	bool returned = node_to_llvm_ir(node->child);

	RESTORE_SUB_SCOPE;

	if (!returned && func->return_type) {
		PANIC(ERR_EXPECTED_RETURN, {
			.real = func->name
		});
	}
	if (returned && !func->return_type) {
		PANIC(ERR_NO_VOID_RETURN, {
			.real = func->name
		});
	}

	if (!func->return_type) {
		LLVMBuildRetVoid(BUILDER);
	}

	LLVMPositionBuilderAtEnd(BUILDER, current_block);

	CURRENT_FUNC = old_func;
}
