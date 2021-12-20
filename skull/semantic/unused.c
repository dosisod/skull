#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/semantic/func.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/unused.h"

bool check_unused_symbols(const AstNode *node) {
	const AstNode *head = node;
	bool err = false;

	while (head) {
		if (head->type == AST_NODE_VAR_DEF) {
			const Variable *var = head->var_def->var;

			if (!var || BUILD_DATA.quiet) {
				head = head->next;
				continue;
			}

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
			const FunctionDeclaration *func = head->func_proto->func;

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
