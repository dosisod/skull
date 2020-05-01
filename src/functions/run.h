#include <string.h>

#include "./print.h"
#include "./dll.h"

/*
Try to run function `name`.
*/
void run_function(const char *function_name, const char *data) {
	if (strcmp(function_name, "print")==0) {
		print(data);
	}
	else if (strcmp(function_name, "dll_run")==0) {
		dll_run(data);
	}
}
