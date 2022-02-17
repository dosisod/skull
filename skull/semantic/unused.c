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

			const Symbol *symbol = head->var_def->symbol;
			const Variable *var = symbol->var;

			if (!var->is_const && !var->was_reassigned) {
				FMT_WARN(err, WARN_VAR_NOT_CONST, {
					.var = var, .loc = &symbol->location
				});
			}
			if (!var->was_read && !var->is_exported) {
				FMT_WARN(err, WARN_VAR_UNUSED, {
					.var = var, .loc = &symbol->location
				});
			}
		}

		else if (head->type == AST_NODE_FUNCTION_PROTO) {
			const Symbol *symbol = head->func_proto->symbol;

			if (!symbol->func->was_called) {
				FMT_WARN(err, WARN_FUNC_UNUSED, {
					.str = symbol->name, .loc = &symbol->location
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
