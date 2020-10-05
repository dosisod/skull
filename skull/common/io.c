#include <stdlib.h>

#include "skull/common/malloc.h"

#include "skull/common/io.h"

/*
Return string read from file descriptor `fd`.
*/
char *read_file(FILE *fd) {
	fseek(fd, 0, SEEK_END);

	const long len = ftell(fd);
	if (len == -1) {
		return NULL;
	}

	fseek(fd, 0, SEEK_SET);

	char *str = malloc((size_t)len + 1);
	fread(str, 1, (size_t)len, fd);

	str[len] = '\0';
	return str;
}