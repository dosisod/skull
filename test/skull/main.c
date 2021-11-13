#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include "parse/ast_node.h"
#include "parse/classify.h"
#include "parse/token.h"

#include "semantic/scope.h"
#include "semantic/types.h"
#include "semantic/variable.h"
#include "semantic/verify.h"

#include "common/errors.h"
#include "common/hashtable.h"
#include "common/str.h"
#include "common/vector.h"

#include "codegen/c/assign.h"
#include "codegen/c/core.h"
#include "codegen/c/expr.h"
#include "codegen/c/flow.h"

#include "../testing.h"

#include "skull/common/local.h"

_Bool SKULL_TESTING = 1;

int main(void) {
	setup_locale();

	_Bool passed = 1;

	testing_test_self(&passed);

	ast_node_test_self(&passed);
	classifier_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);

	error_test_self(&passed);
	hashtable_test_self(&passed);
	str_test_self(&passed);
	vector_test_self(&passed);

	scope_test_self(&passed);
	variable_test_self(&passed);
	semantic_verify_test_self(&passed);

	codegen_c_expr_test_self(&passed);
	codegen_c_assign_test_self(&passed);
	codegen_c_flow_test_self(&passed);
	codegen_c_core_test_self(&passed);

	if (passed) {
		puts("All tests passed!");
	}
	else {
		puts("\n1 or more errors occurred.");
	}
	return !passed;
}
