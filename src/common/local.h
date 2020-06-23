#pragma once

#include <assert.h>
#include <locale.h>

//if system local is not set, default to "en_US.UTF8"
//throw assertion if system does not support UTF-32

#ifndef __STDC_UTF_32__
#define __STDC_UTF_32__ 0
#endif

#define SETUP_LOCALE() \
	static_assert(__STDC_UTF_32__, "Your system does not support UTF-32 for 32 bit char types, we cannot ensure Unicode operations will be handled correctly"); \
	const char *loc=""; \
	if (strcmp("C", setlocale(LC_CTYPE, ""))==0) { \
		loc="en_US.UTF8"; \
	} \
	if (!setlocale(LC_CTYPE, loc)) { \
		puts("cannot set type"); \
	}

