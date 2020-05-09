#include <locale.h>
#include <stdbool.h>
#include <stdio.h>

#include "parse/constants.h"
#include "parse/tokenize.h"
#include "parse/classify.h"
#include "parse/types.h"

#include "eval/eval_bool.h"
#include "eval/variable.h"

#include "common/str.h"

int main() {
	if (!setlocale(LC_CTYPE, "")) {
		printf("Could not set locale.");
		return 1;
	}

	bool passed=true;

	testing_test_self(&passed);
	constants_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);
	classifier_test_self(&passed);

	eval_bool_test_self(&passed);
	variable_test_self(&passed);

	str_test_self(&passed);

	if (!passed) {
		printf("\n1 or more errors occurred.\n");
		return 1;
	}

	return 0;
}
