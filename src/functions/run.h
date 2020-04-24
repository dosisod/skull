#include <string.h>

#include "./print.h"

void run_function(const char *function_name, const char *data) {
	if (strcmp(function_name, "print")==0) {
		print(data);
	}
}
