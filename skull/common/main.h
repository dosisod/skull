#include <errno.h>
#include <stdio.h>
#include <string.h>

#define HANDLE_MAIN \
	if (argc > 2) { \
		puts("too many arguments passed, exiting"); \
		return 1; \
	} \
	if (strcmp("-v", argv[1]) == 0) { \
		puts("Skull " SKULL_VERSION); \
		return 0; \
	} \
	if (!strrstr(argv[1], ".sk")) { \
		puts("missing required \".sk\" extension, exiting"); \
		return 1; \
	} \
	if (strrstr(argv[1], ".sk") == argv[1] || strrstr(argv[1], "/.sk")) { \
		puts("\".sk\" is not a valid name, exiting"); \
		return 1; \
	} \
	errno = 0; \
	FILE *f = fopen(argv[1], "re"); \
	if (!f) { \
		if (errno==EACCES) { \
			printf("cannot open \"%s\", permission denied\n", argv[1]); \
		} \
		else if (errno==ENOENT) { \
			printf("\"%s\" was not found, exiting\n", argv[1]); \
		} \
		return 1; \
	}
