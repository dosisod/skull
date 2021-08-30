#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

/*
Create Skull filename based on `filename` with extention `ext`.
*/
char *gen_filename(const char *filename, const char *ext) {
	const size_t len = strlen(filename);
	const size_t ext_len = strlen(ext);
	char *const new_filename = Malloc(len + 3 + ext_len);

	const char *const slash_pos = strrchr(filename, '/');
	if (!slash_pos) {
		new_filename[0] = '.';
		strncpy(new_filename + 1, filename, len + 1);
	}
	else {
		const long offset = slash_pos - filename;

		strncpy(new_filename, filename, len + 1);
		new_filename[offset + 1] = '.';
		strncpy(
			new_filename + offset + 2,
			slash_pos + 1, len - (size_t)offset
		);
	}

	new_filename[len + 1] = '.';
	strncpy(new_filename + len + 2, ext, ext_len + 1);

	return new_filename;
}
