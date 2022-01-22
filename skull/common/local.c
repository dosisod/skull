#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/local.h"

#ifndef __STDC_UTF_32__
#define __STDC_UTF_32__ 0
#endif

/*
If system local is not set, default to "C.UTF-8". Throw assertion if system
does not support UTF-32, and exit if unable to setup a UTF-8 environment.
*/
void setup_locale(void) {
	static_assert(__STDC_UTF_32__,
		"Your system does not support UTF-32 for 32 bit char types, we"
		"cannot ensure Unicode operations will be handled correctly"
	);

	const char *locale = "";
	const char *set_locale = setlocale(LC_CTYPE, "");

	if (!set_locale || strcmp("C", set_locale) == 0) {
		locale = "C.UTF-8";
	}
	if (!setlocale(LC_CTYPE, locale)) {
		fprintf(stderr, "Cannot setup locale for UTF-8 environment\n");
		exit(1);
	}
}
