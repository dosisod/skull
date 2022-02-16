#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/semantic/func.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/unused.h"

bool check_unused_symbols(const AstNode *node) {
	if (BUILD_DATA.quiet) return true;

	const AstNode *head = node;
	bool err = false;

	while (head) {
		if (head->type == AST_NODE_VAR_DEF) {
			if (!head->var_def->symbol) {
				head = head->next;
				continue;
			}

			const Variable *var = head->var_def->symbol->var;

			if (!var->is_const && !var->was_reassigned) {
				FMT_WARN(err, WARN_VAR_NOT_CONST, {
					.var = var, .loc = &var->location
				});
			}
			if (!var->was_read && !var->is_exported) {
				FMT_WARN(err, WARN_VAR_UNUSED, {
					.var = var, .loc = &var->location
				});
			}
		}

		else if (head->type == AST_NODE_FUNCTION_PROTO) {
			const FunctionDeclaration *func = head->func_proto->symbol->func;

			if (!func->was_called) {
				FMT_WARN(err, WARN_FUNC_UNUSED, {
					.str = func->name, .loc = &func->location
				});
			}
		}

		else if (head->child) {
			err &= check_unused_symbols(head->child);
		}

		head = head->next;
	}

	return !err;
}
