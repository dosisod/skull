#pragma once

#include <locale.h>

//if system local is not set, default to "en_US.UTF8"

#define SETUP_LOCALE() \
	const char *loc=""; \
	if (strcmp("C", setlocale(LC_CTYPE, ""))==0) { \
		loc="en_US.UTF8"; \
	} \
	if (!setlocale(LC_CTYPE, loc)) { \
		puts("cannot set type"); \
	}

