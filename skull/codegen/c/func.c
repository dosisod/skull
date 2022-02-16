#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/core.h"
#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/common/str.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/func.h"

static char *gen_param_c(AstNodeFunctionParam *);

CStmt gen_stmt_func_call_c(const AstNode *node, SkullStateC *state) {
	CExpr expr = gen_expr_c(node->expr, state);
	CStmt out = uvsnprintf("%s;", expr);

	free(expr);
	return out;
}

void gen_function_def_c(const AstNode *node, SkullStateC *state) {
	char *func = gen_function_prototype_c(node, state);

	char *old_globals = state->globals;
	if (old_globals) {
		state->globals = uvsnprintf("%s\n%s", state->globals, func);

		free(old_globals);
		free(func);
	}
	else state->globals = func;
}

char *gen_function_prototype_c(const AstNode *node, SkullStateC *state) {
	FunctionDeclaration *func = node->func_proto->symbol->func;
	AstNodeFunctionParam **params = func->params;
	char *param_list = NULL;
	unsigned short i = 0;

	while (i < func->num_params) {
		char *param = gen_param_c(*params);

		if (param_list) {
			char *old_list = param_list;
			param_list = uvsnprintf("%s, %s", old_list, param);
			free(param);
			free(old_list);
		}
		else {
			param_list = param;
		}

		params++;
		i++;
	}

	char *out = NULL;
	param_list = param_list ? param_list : (char[]){"void"};

	if (!func->is_external) {
		unsigned indent_lvl = state->indent_lvl;
		state->indent_lvl = 0;
		char *tree = gen_tree_c(node->child, state);
		state->indent_lvl = indent_lvl;

		out = uvsnprintf(
			func->is_export ?
				"%s %s(%s) {\n%s\n}" :
				"static %s %s(%s) {\n%s\n}",
			skull_type_to_c_type(func->return_type),
			func->linkage_name,
			param_list,
			tree
		);

		free(tree);
	}
	else {
		out = uvsnprintf(
			"%s %s(%s);",
			skull_type_to_c_type(func->return_type),
			func->linkage_name,
			param_list
		);
	}

	if (func->num_params) free(param_list); // NOLINT
	return out;
}

static char *gen_param_c(AstNodeFunctionParam *param) {
	return uvsnprintf(
		"%s %s",
		skull_type_to_c_type(param->symbol->var->type),
		param->symbol->var->linkage_name
	);
}
