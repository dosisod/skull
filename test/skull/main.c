#include <signal.h>
#include <stdio.h>

#include "parse/ast/node.h"
#include "parse/classify.h"
#include "parse/tokenize.h"

#include "eval/eval_add.h"
#include "eval/eval_assign.h"
#include "eval/eval_div.h"
#include "eval/eval_float.h"
#include "eval/eval_integer.h"
#include "eval/eval_mult.h"
#include "eval/eval_sub.h"
#include "eval/scope.h"
#include "eval/types/types.h"
#include "eval/variable.h"

#include "common/errors.h"
#include "common/str.h"
#include "common/wegex.h"

#include "skull/common/local.h"

void segfault_handler() {
	// add newline when segfault occurs so you can see what file cause it
	putchar('\n');
}

int main(void) {
	SETUP_LOCALE();

	_Bool passed = 1;

	signal(SIGSEGV, segfault_handler);

	testing_test_self(&passed);

	ast_node_test_self(&passed);
	classifier_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);

	error_test_self(&passed);

	scope_test_self(&passed);
	eval_add_test_self(&passed);
	eval_assign_test_self(&passed);
	eval_div_test_self(&passed);
	eval_float_test_self(&passed);
	eval_integer_test_self(&passed);
	eval_sub_test_self(&passed);
	eval_mult_test_self(&passed);
	variable_test_self(&passed);

	str_test_self(&passed);
	wegex_test_self(&passed);

	if (!passed) {
		puts("\n1 or more errors occurred.");
	}
	return !passed;
}
