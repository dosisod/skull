#include <string.h>

#include "./../../src/common/str.h"
#include "./dll.h"
#include "./print.h"

/*
Try to run function `name`.
*/
void run_function(const char *function_name, const char *data) {
	if (samestr(function_name, "print")) {
		print(data);
	}
	else if (samestr(function_name, "dll_run")) {
		dll_run(data);
	}
}
