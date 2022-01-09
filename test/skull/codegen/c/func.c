#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/func.h"
#include "skull/codegen/c/types.h"
#include "skull/semantic/func.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

#include "test/skull/codegen/c/func.h"

static bool test_no_arg_func_decl(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.name = (char[]){"f"},
		.return_type = &TYPE_VOID,
		.is_external = true
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->func = func;

	char *str = gen_function_prototype_c(node);

	ASSERT_TRUTHY(strcmp(str, "void f(void);") == 0);

	free(str);
	PASS;
}

static bool test_single_arg_func_decl(void) {
	Variable *param = make_variable(&TYPE_INT, U"x", true);

	FunctionDeclaration *func = &(FunctionDeclaration){
		.name = (char[]){"f"},
		.return_type = &TYPE_VOID,
		.is_external = true,
		.num_params = 1,
		.params = (AstNodeFunctionParam*[]){
			&(AstNodeFunctionParam){ .var = param }
		}
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->func = func;

	char *str = gen_function_prototype_c(node);

	ASSERT_TRUTHY(strcmp(str, "void f("TYPE_INT_C" x);") == 0);

	free(str);
	free_variable(param);
	PASS;
}

static bool test_many_arg_func_decl(void) {
	Variable *param_x = make_variable(&TYPE_INT, U"x", true);
	Variable *param_y = make_variable(&TYPE_INT, U"y", true);

	FunctionDeclaration *func = &(FunctionDeclaration){
		.name = (char[]){"f"},
		.return_type = &TYPE_VOID,
		.is_external = true,
		.num_params = 2,
		.params = (AstNodeFunctionParam*[]){
			&(AstNodeFunctionParam){ .var = param_x },
			&(AstNodeFunctionParam){ .var = param_y }
		}
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, true, false);
	node->func_proto->func = func;

	char *str = gen_function_prototype_c(node);

	ASSERT_TRUTHY(strcmp(str, "void f("TYPE_INT_C" x, "TYPE_INT_C" y);") == 0);

	free(str);
	free_variable(param_x);
	free_variable(param_y);
	PASS;
}

static bool test_func_with_body(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.name = (char[]){"f"},
		.return_type = &TYPE_VOID
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, false, false);
	node->child = AST_NODE_NOOP();
	node->func_proto->func = func;

	char *str = gen_function_prototype_c(node);

	ASSERT_TRUTHY(strcmp(str, "static void f(void) {\n\t(void)0;\n}") == 0);

	free(str);
	PASS;
}

static bool test_func_static(void) {
	FunctionDeclaration *func = &(FunctionDeclaration){
		.name = (char[]){"f"},
		.is_export = true,
		.return_type = &TYPE_VOID
	};

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(&(Token){0}, false, true);
	node->child = AST_NODE_NOOP();
	node->func_proto->func = func;

	char *str = gen_function_prototype_c(node);

	ASSERT_TRUTHY(strcmp(str, "void f(void) {\n\t(void)0;\n}") == 0);

	free(str);
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
