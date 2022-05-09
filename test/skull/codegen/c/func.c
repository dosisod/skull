#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/func.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/semantic/func.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

#include "test/skull/codegen/c/func.h"

static bool test_no_arg_func_decl(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.return_type = &TYPE_VOID,
		.is_external = true
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->symbol->func = func;
	node->func_proto->symbol->name = (char[]){"f"};

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_function_prototype_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "void f(void);") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_single_arg_func_decl(void) {
	Variable *param = make_variable(&TYPE_INT, U"x", true);

	FunctionDeclaration *func = &(FunctionDeclaration){
		.return_type = &TYPE_VOID,
		.is_external = true,
		.num_params = 1,
		.params = (AstNodeFunctionParam*[]){
			&(AstNodeFunctionParam){
				.symbol = &(Symbol){ .var = param },
				.location = NULL
			}
		}
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->symbol->func = func;
	node->func_proto->symbol->name = (char[]){"f"};

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_function_prototype_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "void f("TYPE_INT_C" x);") == 0);

	free(str);
	free_variable(param);
	free_c_state(state);
	PASS;
}

static bool test_many_arg_func_decl(void) {
	Variable *param_x = make_variable(&TYPE_INT, U"x", true);
	Variable *param_y = make_variable(&TYPE_INT, U"y", true);

	FunctionDeclaration *func = &(FunctionDeclaration){
		.return_type = &TYPE_VOID,
		.is_external = true,
		.num_params = 2,
		.params = (AstNodeFunctionParam*[]){
			&(AstNodeFunctionParam){
				.symbol = &(Symbol){ .var = param_x },
				.location = NULL
			},
			&(AstNodeFunctionParam){
				.symbol = &(Symbol){ .var = param_y },
				.location = NULL
			}
		}
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->symbol->func = func;
	node->func_proto->symbol->name = (char[]){"f"};

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_function_prototype_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "void f("TYPE_INT_C" x, "TYPE_INT_C" y);") == 0);

	free(str);
	free_variable(param_x);
	free_variable(param_y);
	free_c_state(state);
	PASS;
}

static bool test_func_with_body(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.return_type = &TYPE_VOID
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, false, false);
	node->child = AST_NODE_NOOP();
	node->func_proto->symbol->func = func;
	node->func_proto->symbol->name = (char[]){"f"};

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_function_prototype_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "static void f(void) {\n\t(void)0;\n}") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_func_static(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.is_export = true,
		.return_type = &TYPE_VOID
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, false, true);
	node->child = AST_NODE_NOOP();
	node->func_proto->symbol->func = func;
	node->func_proto->symbol->name = (char[]){"f"};
	node->func_proto->symbol->linkage_name = (char[]){"exported_name"};

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_function_prototype_c(node, state);

	const char *expected = "void f(void) __asm__(\"exported_name\");\nvoid f(void) {\n\t(void)0;\n}";

	ASSERT_TRUTHY(strcmp(str, expected) == 0);

	free(str);
	free_c_state(state);
	PASS;
}

void codegen_c_func_test_self(bool *pass) {
	RUN_ALL(
		test_no_arg_func_decl,
		test_single_arg_func_decl,
		test_many_arg_func_decl,
		test_func_with_body,
		test_func_static
	)
}
