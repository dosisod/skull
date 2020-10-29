#include <signal.h>
#include <stdio.h>
#include <stdnoreturn.h>

#include "parse/ast/node.h"
#include "parse/classify.h"
#include "parse/tokenize.h"

#include "eval/scope.h"
#include "eval/types/types.h"
#include "eval/variable.h"

#include "common/errors.h"
#include "common/str.h"

#include "skull/common/local.h"

noreturn void segfault_handler() {
	// add newline when segfault occurs so you can see what file cause it
	putchar('\n');

	exit(1);
}

int main(void) {
	SETUP_LOCALE();

	_Bool passed = 1;

	struct sigaction sa = {0};
	sa.sa_handler = &segfault_handler;
	sigaction(SIGSEGV, &sa, NULL);

	testing_test_self(&passed);

	ast_node_test_self(&passed);
	classifier_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);

	error_test_self(&passed);

	scope_test_self(&passed);
	variable_test_self(&passed);

	str_test_self(&passed);

	if (!passed) {
		puts("\n1 or more errors occurred.");
	}
	return !passed;
}
