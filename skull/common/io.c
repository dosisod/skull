#include <stdlib.h>

#include "skull/common/malloc.h"

#include "skull/common/io.h"

/*
Return string read from file descriptor `fd`.
*/
char *read_file(FILE *const fd) {
	fseek(fd, 0, SEEK_END);

	const long len = ftell(fd);
	if (len == -1) {
		return NULL;
	}

	fseek(fd, 0, SEEK_SET);

	char *const str = Malloc((size_t)len + 1);
	const size_t read = fread(str, 1, (size_t)len, fd);

	if (!read) {
		free(str);
		return NULL;
	}

	str[len] = '\0';
	return str;
}
