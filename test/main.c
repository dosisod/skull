#include <stdbool.h>
#include <stdio.h>

#include "parse/constants.h"
#include "testing.h"

int main() {
	bool failed=false;

	testing_test_self(&failed);
	constants_test_self(&failed);

	if (failed) {
		printf("\n1 or more errors occurred.\n");
	}

	return failed;
}
