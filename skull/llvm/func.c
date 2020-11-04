#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"
#include "skull/parse/classify.h"

#include "skull/llvm/func.h"

extern LLVMBuilderRef builder;
extern LLVMModuleRef module;
extern Scope *scope;

ExternalFunction *external_functions = NULL;

/*
Store function name of externaly declared function in `node`.
*/
void declare_external_function(AstNode *node) {
	char32_t *const wide_func_name = token_str(node->token->next);
	char *const func_name = c32stombs(wide_func_name);

	ExternalFunction *f;
	f = malloc(sizeof *f);
	DIE_IF_MALLOC_FAILS(f);

	f->name = func_name;
	f->next = NULL;

	LLVMTypeRef *params = NULL;
	f->num_params = 0;

	if (node->token->next->next->next->token_type != TOKEN_PAREN_CLOSE) {
		char *type_name = token_mbs_str(node->token->next->next->next->next);

		f->num_params = 1;
		f->param_types = find_type(type_name);

		LLVMTypeRef llvm_param_type = f->param_types->llvm_type();
		params = &llvm_param_type;
	}

	LLVMTypeRef type = LLVMFunctionType(
		LLVMVoidType(),
		params,
		f->num_params,
		false
	);
	f->type = type;

	LLVMValueRef function = LLVMAddFunction(
		module,
		func_name,
		type
	);
	LLVMSetLinkage(function, LLVMExternalLinkage);
	f->function = function;

	ExternalFunction *head = external_functions;
	while (head) {
		if (strcmp(func_name, head->name) == 0) {
			PANIC("cannot redeclare external function \"%s\"\n", { .str = wide_func_name });
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
		external_functions = f;
	}
}

/*
Builds a function declaration from `node`.
*/
void llvm_make_function(AstNode *node) {
	char32_t *const wide_func_name = token_str(node->token);
	char *const func_name = c32stombs(wide_func_name);

	ExternalFunction *current_function = external_functions;
	while (current_function) {
		if (strcmp(current_function->name, func_name) == 0) {
			break;
		}
		current_function = current_function->next;
	}
	free(func_name);

	if (!current_function) {
		PANIC("external function \"%s\" missing external declaration\n", { .str = wide_func_name });
	}
	free(wide_func_name);

	LLVMValueRef params = NULL;

	if (current_function->num_params == 1) {
		if (node->token->next->next->token_type == TOKEN_IDENTIFIER) {
			SCOPE_FIND_VAR(var_found, node->token->next->next, lookup);
			free(lookup);

			if (var_found->type != current_function->param_types) {
				PANIC(ERR_TYPE_MISMATCH, {
					.real = strdup(current_function->param_types->name)
				});
			}

			LLVMValueRef tmp_store = LLVMBuildLoad2(
				builder,
				var_found->type->llvm_type(),
				var_found->alloca,
				""
			);

			params = tmp_store;
		}
		else if (current_function->param_types != token_type_to_type(node->token->next->next)) {
			PANIC(ERR_FUNC_TYPE_MISMATCH, {
				.real = strdup(current_function->param_types->name)
			});
		}
		else {
			params = llvm_parse_token(node->token->next->next);
		}
	}

	LLVMBuildCall2(
		builder,
		current_function->type,
		current_function->function,
		&params,
		current_function->num_params,
		""
	);
}
