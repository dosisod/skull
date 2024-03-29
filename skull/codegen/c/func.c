#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/core.h"
#include "skull/codegen/c/debug.h"
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
	Symbol *symbol = node->func_proto->symbol;
	FunctionDeclaration *func = symbol->func;
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

	char *return_type = skull_type_to_c_type(func->return_type);

	char *fwd_decl = NULL;
	if (func->is_export) {
		fwd_decl = uvsnprintf(
			"%s %s(%s) __asm__(\"%s\");\n",
			return_type,
			symbol->name,
			param_list,
			symbol->linkage_name
		);
	}

	if (func->is_external) {
		if (func->is_export) out = fwd_decl;
		else {
			out = uvsnprintf(
				"%s %s(%s);",
				return_type,
				symbol->name,
				param_list
			);
		}
	}
	else {
		unsigned indent_lvl = state->indent_lvl;
		state->indent_lvl = 0;
		char *tree = gen_tree_c(node->child, state);
		state->indent_lvl = indent_lvl;

		out = uvsnprintf(
			func->is_export ?
				"%s%s %s(%s) {\n%s\n}" :
				"%sstatic %s %s(%s) {\n%s\n}",
			func->is_export ? fwd_decl : "",
			return_type,
			symbol->name,
			param_list,
			tree
		);

		out = apply_debug_info(out, node);

		free(tree);
	}

	free(return_type);
	if (fwd_decl != out) free(fwd_decl);
	if (func->num_params) free(param_list); // NOLINT
	return out;
}

static char *gen_param_c(AstNodeFunctionParam *param) {
	char *return_type = skull_type_to_c_type(param->symbol->var->type);

	char *tmp = uvsnprintf("%s %s", return_type, param->symbol->var->name);

	free(return_type);
	return tmp;
}
