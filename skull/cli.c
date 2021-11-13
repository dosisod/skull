#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/common/io.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/real_main.h"


#ifndef SKULL_VERSION
#define SKULL_VERSION "<unknown>"
#endif

#ifndef RELEASE
#define RELEASE 0
#endif


static int usage(void);
static int version(void);
static void set_bool_flag(bool *, const char *);
static int handle_file(void);
static int run_llc(void);
static int run_cc(char *);
static bool is_directory(const char *);
static const char *module_shim;
static bool parse_long_option(const char *);
static char *squash_argv(char *[]);
static int sh(char *[]);
static int shell(char *);
static int handle_args(int, char *[]);
static void cleanup(void);
static noreturn void bail(int);
static char *get_llc_binary(void);

char *filename = NULL;
char *args = (char[]){0};
char *out_filename = NULL;

int main(int argc, char *argv[]) {
	if (argc == 1) return usage();

	const int exit_code = handle_args(argc - 1, argv + 1);
	if (exit_code) return exit_code;

	if (!filename) {
		puts("skull: expected filename");
		return 1;
	}

	if (!strrstr(filename, ".sk")) {
		printf("skull: missing required \".sk\" extension, exiting");
		free(out_filename);
		return 1;
	}

	return handle_file();
}

static int handle_args(int argc, char *argv[]) {
	if (argc == 0) return 0;

	if (**argv != '-') {
		filename = argv[0];
		return handle_args(--argc, ++argv);
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
				puts("skull: expected filename after -o");
				bail(1);
			}

			free(out_filename);
			out_filename = strdup(argv[1]);
			BUILD_DATA.out_file = out_filename;
			argc--;
			break;
		}
		case '-': {
			if (argv[0][2]) {
				const bool fail = parse_long_option(argv[0] + 2);
				if (fail) bail(1);

				break;
			}
			if (argc == 1) {
				puts("skull: expected arguments after --");
				bail(1);
			}

			args = squash_argv(argv + 1);
			return 0;
		}
		default: {
			printf("skull: unknown option \"%s\"\n", *argv);
			bail(1);
		}
	}

	return handle_args(--argc, ++argv);
}

static void set_bool_flag(bool *flag, const char *str) {
	if (*flag) {
		printf("skull: %s cannot be used more then once\n", str);
		bail(1);
	}
	*flag = true;
}

static int handle_file(void) {
	char *binary_name = strdup(filename); // NOLINT
	char *last_dot = strrchr(binary_name, '.');
	binary_name[last_dot - binary_name] = '\0';

	if (is_directory(binary_name)) {
		printf("skull: \"%s\" is a directory not a file\n", binary_name);
		free(binary_name);
		return 1;
	}

	const int err = real_main(2, (char *[]){ NULL, filename, NULL });
	if (err || BUILD_DATA.preprocess || BUILD_DATA.c_backend) {
		free(binary_name);
		free(out_filename);
		return err;
	}

	if (BUILD_DATA.preprocess) {
		free(binary_name);
		return 0;
	}

	const int exit_code = run_llc();
	if (exit_code) {
		free(binary_name);
		free(out_filename);
		return exit_code;
	}

	if (BUILD_DATA.asm_backend || BUILD_DATA.compile_only) {
		free(binary_name);
		free(out_filename);
		return 0;
	}

	return run_cc(binary_name);
}

static int run_llc(void) {
	if (!out_filename) {
		out_filename = gen_filename(
			filename,
			BUILD_DATA.asm_backend ? "s" : "o"
		);
	}
	if (!BUILD_DATA.asm_backend && strcmp(out_filename, "-") == 0) {
		puts("skull: cannot print binary file to stdout");
		return 1;
	}

	char *llc_cmd = get_llc_binary();
	if (!llc_cmd) return 1;

	char *llvm_file = gen_filename(filename, "ll");
	char *filetype = strdup(BUILD_DATA.asm_backend ?
		"-filetype=asm" :
		"-filetype=obj"
	);

	static char dash_o[] = "-o";

	char *_args[] = {
		llc_cmd,
		llvm_file,
		filetype,
		dash_o,
		out_filename,
		NULL
	};

	const int exit_code = sh(_args);

	errno = 0;
	remove(llvm_file);

	free(llvm_file);
	free(filetype);

	if (errno) {
		perror("remove");
		return 1;
	}

	return exit_code;
}

#define CHECK_CMD(_cmd) { \
	static char check_cmd[] = "which "_cmd" > /dev/null"; \
	static char cmd[] = _cmd; \
	const bool cmd_exists = !shell(check_cmd); \
	if (cmd_exists) return cmd; \
}

static char *get_llc_binary(void) {
	CHECK_CMD("llc-10")
	CHECK_CMD("llc")

	puts("skull: llc-10 command not found");

	return NULL;
}
#undef CHECK_CMD

static int run_cc(char *binary_name) {
	char *module_name = create_main_func_name(filename);

	char *shim = uvsnprintf(
		module_shim,
		module_name,
		out_filename,
		binary_name,
		args
	);
	free(module_name);
	free(binary_name);

	const int exit_code = shell(shim);

	remove(out_filename);
	free(out_filename);
	free(shim);
	if (*args) free(args);

	return exit_code;
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

	// this `popen` call is insecure, since the HOME variable can be modified
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
		puts("skull: failed to get version (is git installed?)");
		return 1;
	}
#endif

	printf("\033[92mSkull\033[0m %s\n", version_buf);
	return 0;
}

static bool is_directory(const char *path) {
	 struct stat statbuf;
	 if (stat(path, &statbuf) != 0) return false;

	 return S_ISDIR(statbuf.st_mode);
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
		printf("skull: unknown option \"--%s\"\n", arg);
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

static int sh(char *argv[]) {
	errno = 0;
	const pid_t pid = fork();
	if (errno) {
		perror("fork");
		return 1;
	}

	if (pid == 0) {
		errno = 0;
		execvp(argv[0], argv);
		if (errno) {
			perror("execvp");
			return 1;
		}
	}

	int status = 0;
	wait(&status);

	return WEXITSTATUS(status);
}

static int shell(char *cmd) {
	static char shell[] = "/bin/sh";
	static char dash_c[] = "-c";

	return sh((char *[]){ shell, dash_c, cmd, NULL });
}

static void bail(int exit_code) {
	cleanup();
	exit(exit_code);
}

static void cleanup(void) {
	free(out_filename);
	if (*args) free(args);
}


#include "skull/cc_shim.h"
static const char *module_shim = SHIM;
