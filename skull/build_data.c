#include <stdbool.h>
#include <stdlib.h>

#include "skull/build_data.h"

BuildData BUILD_DATA;

void setup_flags(void) {
	char *c_backend = getenv("C_BACKEND");
	BUILD_DATA.c_backend = c_backend ? *c_backend == '1' : false;

	char *debug = getenv("DEBUG");
	BUILD_DATA.debug = debug ? *debug == '1' : false;

	BUILD_DATA.error_file = getenv("ERROR_FILE");
}
