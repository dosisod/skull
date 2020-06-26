#include <locale.h>
#include <stdbool.h>
#include <stdio.h>

#include "parse/ast/node.h"
#include "parse/classify.h"
#include "parse/constants.h"
#include "parse/tokenize.h"
#include "parse/types.h"

#include "errors.h"

#include "eval/context.h"
#include "eval/eval_add.h"
#include "eval/eval_assign.h"
#include "eval/eval_bool.h"
#include "eval/eval_float.h"
#include "eval/eval_integer.h"
#include "eval/repl.h"
#include "eval/variable.h"

#include "common/color.h"
#include "common/str.h"
#include "common/wegex.h"

#include "../src/common/local.h"

int main(void) {
	SETUP_LOCALE();

	bool passed=true;

	testing_test_self(&passed);

	ast_node_test_self(&passed);
	constants_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);
	classifier_test_self(&passed);

	error_test_self(&passed);

	eval_assign_test_self(&passed);
	eval_bool_test_self(&passed);
	eval_float_test_self(&passed);
	eval_integer_test_self(&passed);
	variable_test_self(&passed);
	context_test_self(&passed);
	eval_add_test_self(&passed);
	repl_test_self(&passed);

	color_test_self(&passed);
	str_test_self(&passed);
	wegex_test_self(&passed);

	if (!passed) {
		puts("\n1 or more errors occurred.");
		return 1;
	}

	return 0;
}
