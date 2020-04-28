#include <stdbool.h>
#include <stdio.h>

#include "parse/constants.h"
#include "parse/tokenize.h"
#include "parse/types.h"

int main() {
	bool passed=true;

	testing_test_self(&passed);
	constants_test_self(&passed);
	tokenizer_test_self(&passed);
	types_test_self(&passed);

	if (!passed) {
		printf("\n1 or more errors occurred.\n");
		return 1;
	}

	return 0;
}
