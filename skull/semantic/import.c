#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"

#include "skull/semantic/import.h"

static bool had_load_module_error(char *);
static char *filename_for_module(char *);
static char *current_file_dir(void);

bool validate_stmt_import(const AstNode *node) {
	char *module_name = token_to_mbs_str(node->token->next);
	char *directory = current_file_dir();

	errno = 0;
	const int dir_fd = open(directory, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
	free(directory);

	if (had_load_module_error(module_name)) {
		close(dir_fd);
		return false;
	}

	char *filename = filename_for_module(module_name);

	int file_fd = openat(dir_fd, filename, O_RDONLY | O_CLOEXEC);
	free(filename);

	if (had_load_module_error(module_name)) {
		close(dir_fd);
		return false;
	}

	close(file_fd);
	close(dir_fd);

	free(module_name);

	return true;
}

static char *filename_for_module(char *module_name) {
	const size_t len = strlen(module_name);
	char *filename = Malloc(len + 4);

	strncpy(filename, module_name, len);
	strncpy(filename + len, ".sk", 4);
	filename[len + 3] = '\0';

	return filename;
}

static char *current_file_dir(void) {
	char *directory = strdup(BUILD_DATA.filename);
	char *slash = strrchr(directory, '/');
	if (slash && directory != slash) {
		*slash = '\0';
	}
	else {
		directory[0] = '.';
		directory[1] = '\0';
	}

	return directory;
}

/*
Check if there was an error loading `module_name`. This function takes
ownership of `module_name` if there was an error.
*/
static bool had_load_module_error(char *module_name) {
	if (errno) {
		FMT_ERROR(
			ERR_LOAD_MODULE,
			{ .real = module_name },
			{ .str = strerror(errno) }
		);

		return true;
	}

	return false;
}
