#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include "parse/ast_node.h"
#include "parse/classify.h"
#include "parse/token.h"

#include "compiler/scope.h"
#include "compiler/types/types.h"
#include "compiler/variable.h"

#include "common/errors.h"
#include "common/hashtable.h"
#include "common/str.h"
#include "common/vector.h"

#include "skull/common/local.h"

noreturn void segfault_handler() {
	fprintf(stderr, "\n\nsegfault occurred in function `%s`, exiting\n\n", FUNC_NAME);

	exit(1);
}

int main(void) {
	setup_locale();

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
	hashtable_test_self(&passed);
	str_test_self(&passed);
	vector_test_self(&passed);

	scope_test_self(&passed);
	variable_test_self(&passed);

	if (!passed) {
		puts("\n1 or more errors occurred.");
	}
	return !passed;
}
