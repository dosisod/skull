#include <stdbool.h>
#include <stdio.h>

#include "parse/constants.h"
#include "parse/tokenize.h"
#include "parse/classify.h"
#include "parse/types.h"

#include "eval/eval_if.h"

int main() {
	bool passed=true;

	testing_test_self(&passed);
	constants_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);
	classifier_test_self(&passed);

	eval_if_test_self(&passed);

	if (!passed) {
		printf("\n1 or more errors occurred.\n");
		return 1;
	}

	return 0;
}
