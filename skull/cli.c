#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/cc.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"


#ifndef SKULL_VERSION
#define SKULL_VERSION "<unknown>"
#endif

#ifndef RELEASE
#define RELEASE 0
#endif


static int usage(void);
static int version(void);
static void set_bool_flag(bool *, const char *);
static bool parse_long_option(const char *);
static char *squash_argv(char *[]);
static int handle_args(int, char *[]);
static void cleanup(void);
static noreturn void bail(int);

int main(int argc, char *argv[]) {
	if (argc == 1) return usage();

	int err = handle_args(argc - 1, argv + 1);
	if (err) return err;

	if (!BUILD_DATA.filename) {
		fprintf(stderr, "skull: expected filename\n");
		return 1;
	}

	if (!strrstr(BUILD_DATA.filename, ".sk")) {
		fprintf(
			stderr, "skull: missing required \".sk\" extension, exiting\n"
		);
		cleanup();
		return 1;
	}

	err = handle_file();
	free(BUILD_DATA.out_file);
	return err;
}

static int handle_args(int argc, char *argv[]) {
	if (argc == 0) return 0;

	if (**argv != '-') {
		BUILD_DATA.filename = argv[0];
		return handle_args(--argc, ++argv);
	}

	if (argv[0][1] && argv[0][1] != '-' && argv[0][2]) {
		fprintf(stderr, "skull: short option must be exactly one character\n");
		bail(1);
	}

	switch (argv[0][1]) {
		case 'h': bail(usage());
		case 'v': bail(version());
		case 'c': {
			set_bool_flag(&BUILD_DATA.compile_only, "-c");
			break;
		}
		case 'S': {
			set_bool_flag(&BUILD_DATA.asm_backend, "-S");
			break;
		}
		case 'E': {
			set_bool_flag(&BUILD_DATA.preprocess, "-E");
			break;
		}
		case 'g': {
			set_bool_flag(&BUILD_DATA.debug, "-g");
			break;
		}
		case 'q': {
			set_bool_flag(&BUILD_DATA.quiet, "-q");
			break;
		}
		case 'o': {
			if (argc == 1) {
				fprintf(stderr, "skull: expected filename after -o\n");
				bail(1);
			}

			free(BUILD_DATA.out_file);
			BUILD_DATA.out_file = strdup(argv[1]);
			argc--;
			argv++;
			break;
		}
		case '-': {
			if (argv[0][2]) {
				const bool fail = parse_long_option(argv[0] + 2);
				if (fail) bail(1);

				break;
			}
			if (argc == 1) {
				fprintf(stderr, "skull: expected arguments after --\n");
				bail(1);
			}

			BUILD_DATA.extra_args = squash_argv(argv + 1);
			return 0;
		}
		default: {
			fprintf(stderr, "skull: unknown option \"%s\"\n", *argv);
			bail(1);
		}
	}

	return handle_args(--argc, ++argv);
}

static void set_bool_flag(bool *flag, const char *str) {
	if (*flag) {
		fprintf(stderr, "skull: %s cannot be used more then once\n", str);
		bail(1);
	}
	*flag = true;
}

static int usage(void) {
printf("skull [-v|--version|-h|--help]\n\n" \
"skull file [-S|-E|-c|--c-backend]\n" \
"           [-gq]\n" \
"           [--werror]\n" \
"           [-o output]\n" \
"           [-- args]\n"
);


	return 0;
}

static int version(void) {
#if RELEASE
	const char *version_buf = SKULL_VERSION;
#else
# define VERSION_MAX 32
	static char version_buf[VERSION_MAX];

	// this `popen` call is insecure, since the PATH variable can be modified
	// to change what the `git` command points to. This is only indented for
	// developers only, and not for release builds. Also, do not run this with
	// elevated privileges when in debug mode!

	errno = 0;
	FILE *f = popen("git describe --always 2>/dev/null", "r"); // NOLINT
	if (errno) {
		perror("popen");
		return 1;
	}

	errno = 0;
	if (!fgets(version_buf, VERSION_MAX, f)) {
		perror("fgets");
	}

	char *newline = strchr(version_buf, '\n');
	if (newline) version_buf[newline - version_buf] = '\0';

	if (WEXITSTATUS(pclose(f))) {
		fprintf(stderr, "skull: failed to get version (is git installed?)\n");
		return 1;
	}
#endif

	printf("\033[92mSkull\033[0m %s\n", version_buf);
	return 0;
}

// return true if an error occurs
static bool parse_long_option(const char *arg) {
	if (strcmp(arg, "version") == 0) bail(version());
	if (strcmp(arg, "help") == 0) bail(usage());
	if (strcmp(arg, "c-backend") == 0) {
		set_bool_flag(&BUILD_DATA.c_backend, "--c-backend");
	}
	else if (strcmp(arg, "werror") == 0) {
		set_bool_flag(&BUILD_DATA.werror, "--werror");
	}
	else {
		fprintf(stderr, "skull: unknown option \"--%s\"\n", arg);
		return true;
	}

	return false;
}

static char *squash_argv(char *argv[]) {
	size_t len = 0;
	char **argv_copy = argv;

	while (*argv) {
		len += strlen(*argv) + 1;
		argv++;
	}

	argv = argv_copy;

	char *out = Malloc(len);
	size_t wrote = 0;

	while (*argv) {
		const size_t current_len = strlen(*argv) + 1;
		memcpy(out + wrote, *argv, current_len);
		wrote += current_len;
		out[wrote - 1] = ' ';
		argv++;
	}

	out[wrote - 1] = '\0';

	return out;
}

static void bail(int exit_code) {
	cleanup();
	exit(exit_code);
}

static void cleanup(void) {
	free(BUILD_DATA.out_file);
	free(BUILD_DATA.extra_args);
}
