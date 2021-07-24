#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/malloc.h"

#include "skull/common/io.h"

/*
Return string read from file descriptor `fd`.
*/
char *file_to_string(FILE *const fd) {
	fseek(fd, 0, SEEK_END);

	const long len = ftell(fd);
	if (len == -1) return NULL;

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

/*
Open `filename`, handle errors if any.

Set `is_RO` to true if file should be opened as read-only.
*/
FILE *open_file(const char *filename, bool is_RO) {
	errno = 0;
	FILE *const f = fopen(filename, is_RO ? "re" : "we");
	if (!f) {
		if (errno == EACCES)
			fprintf(
				stderr,
				"skull: cannot open \"%s\", permission denied\n",
				filename
			);

		else if (errno == ENOENT)
			fprintf(
				stderr,
				"skull: \"%s\" was not found, exiting\n",
				filename
			);

		return NULL;
	}

	return f;
}
