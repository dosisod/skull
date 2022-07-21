#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "skull/common/malloc.h"

#include "skull/common/io.h"

#define CHECK_ERRNO(msg) \
	if (errno) { \
		perror((msg)); \
		return NULL; \
	}

/*
Return string read from file descriptor `fd`.
*/
char *file_to_string(FILE *fd) {
	errno = 0;
	fseek(fd, 0, SEEK_END);
	CHECK_ERRNO("fseek");

	const long len = ftell(fd);
	CHECK_ERRNO("ftell");

	if (!len) return NULL;

	errno = 0;
	fseek(fd, 0, SEEK_SET);
	CHECK_ERRNO("fseek");

	char *str = Malloc((size_t)len + 1);
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

Set `is_ro` to true if file should be opened as read-only.
*/
FILE *open_file(const char *filename, bool is_ro) {
	errno = 0;
	FILE *f = fopen(filename, is_ro ? "re" : "we");

	if (f) return f;

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

	else perror("fopen");

	return NULL;
}

/*
Create Skull filename based on `filename` with extention `ext`.
*/
char *gen_filename(const char *filename, const char *ext) {
	const size_t len = strlen(filename);
	const size_t ext_len = strlen(ext);
	char *new_filename = Malloc(len + 3 + ext_len);

	const char *slash_pos = strrchr(filename, '/');
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

bool is_directory(const char *path) {
	 struct stat statbuf;
	 if (stat(path, &statbuf) != 0) return false;

	 return S_ISDIR(statbuf.st_mode);
}
