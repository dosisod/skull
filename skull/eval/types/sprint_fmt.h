#include <stdio.h>
#include <stdlib.h>

#include "skull/common/malloc.h"

#define SPRINTF_FMT(fmt) \
	int needed=snprintf(NULL, 0, (fmt), data) + 1; \
	if (needed<0) { \
		return NULL; \
	} \
	char *tmp=malloc(sizeof(char) * (unsigned long)needed); \
	DIE_IF_MALLOC_FAILS(tmp); \
	int wrote=snprintf(tmp, (unsigned long)needed, (fmt), data); \
	if (wrote<0) { \
		free(tmp); \
		return NULL; \
	} \

