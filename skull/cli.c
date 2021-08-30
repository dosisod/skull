#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/shared.h"
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
static int handle_file(char *, char *);
static int run_llc(char *);
static int run_cc(char *, char *, char *);
static bool is_directory(const char *);
static const char *module_shim;
static char *squash_argv(char *[]);
static int sh(char *[]);

int main(int argc, char *argv[]) {
	if (argc == 1) return usage();

	char *filename = NULL;
	char *args = (char[]){0};

	for (int arg = 1; arg < argc; arg++) {
		if (*argv[arg] == '-') {
			switch (argv[arg][1]) {
				case 'h': return usage();
				case 'v': return version();
				case 'c': BUILD_DATA.compile_only = true; break;
				case 'S': BUILD_DATA.asm_backend = true; break;
				case 'E': BUILD_DATA.preprocess = true; break;
				case 'o': BUILD_DATA.out_filename = strdup(argv[++arg]); break;
				case '-': {
					args = squash_argv(argv + arg + 1);
					arg = argc;
					break;
				}
				default: {
					printf("skull: unknown option \"%s\"\n", argv[arg]);
					return 1;
				}
			}
		}
		else {
			filename = argv[arg];
		}
	}

	if (!filename) {
		puts("skull: expected filename");
		return 1;
	}

	return handle_file(filename, args);
}

static int handle_file(char *filename, char *args) {
	char *binary_name = strdup(filename);
	char *last_dot = strrchr(binary_name, '.');
	binary_name[last_dot - binary_name] = '\0';

	if (is_directory(binary_name)) {
		printf("skull: \"%s\" is a directory not a file\n", binary_name);
		free(binary_name);
		return 1;
	}

	const int err = real_main(2, (char *[]){ NULL, filename, NULL });
	if (err) {
		free(binary_name);
		return err;
	}

	if (BUILD_DATA.preprocess) {
		free(binary_name);
		return 0;
	}

	const int exit_code = run_llc(filename);
	if (exit_code) {
		free(binary_name);
		return exit_code;
	}

	if (BUILD_DATA.asm_backend || BUILD_DATA.compile_only) {
		free(binary_name);
		return 0;
	}

	return run_cc(filename, binary_name, args);
}

static int run_llc(char *filename) {
	if (!BUILD_DATA.out_filename) {
		BUILD_DATA.out_filename = gen_filename(
			filename,
			BUILD_DATA.asm_backend ? "s" : "o"
		);
	}

	// TODO(dosisod): autodetect which llc to use
	char *llc_cmd = strdup("llc-10");
	char *llvm_file = gen_filename(filename, "ll");
	char *filetype = strdup(BUILD_DATA.asm_backend ?
		"-filetype=asm" :
		"-filetype=obj"
	);

	static char dash_o[] = "-o";

	char *args[] = {
		llc_cmd,
		llvm_file,
		filetype,
		dash_o,
		BUILD_DATA.out_filename,
		NULL
	};

	const int exit_code = sh(args);

	errno = 0;
	remove(llvm_file);

	free(llc_cmd);
	free(llvm_file);
	free(filetype);

	if (errno) {
		perror("remove");
		return 1;
	}

	return exit_code;
}

static int run_cc(char *filename, char *binary_name, char *args) {
	char *module_name = create_main_func_name(filename);

	char *shim = uvsnprintf(
		module_shim,
		module_name,
		BUILD_DATA.out_filename,
		binary_name,
		args
	);
	free(module_name);
	free(binary_name);

	static char shell[] = "/bin/sh";
	static char dash_c[] = "-c";

	const int exit_code = sh((char *[]){ shell, dash_c, shim, NULL });

	remove(BUILD_DATA.out_filename);
	free(BUILD_DATA.out_filename);
	free(shim);
	if (*args) free(args); // NOLINT

	return exit_code;
}

static int usage(void) {
	printf("skull -v\n" \
"skull file [ -S | -E | -c ]\n" \
"           [ -o output ]\n" \
"           [ -- args]\n"
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

	fgets(version_buf, VERSION_MAX, f);

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
		const size_t current_len = strlen(*argv);
		strncpy(out + wrote, *argv, current_len);
		wrote += current_len + 1;
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

static const char *module_shim = \
"echo \'_Bool _strcmp(const char *a, const char *b) {\n" \
"	while (*a && *b) {\n" \
"		if (*a != *b) {\n" \
"			return 0;\n" \
"		}\n" \
"		a++;\n" \
"		b++;\n" \
"	}\n" \
"\n" \
"	return !*a && !*b;\n" \
"}\n" \
"\n" \
"#include <math.h>\n" \
"double _float_pow(double base, double exp) {\n" \
"	return pow(base, exp);\n" \
"}\n" \
"\n" \
"#include <stdint.h>\n" \
"int64_t _int_pow(int64_t base, int64_t exp) {\n" \
"	int64_t result = base;\n" \
"	for (int64_t i = 1; i < exp; i++) result *= base;\n" \
"	return result;\n" \
"}\n" \
"\n" \
"int main(void) {\n" \
"	int x(void) __asm__(\"%s\"); return x();\n" \
"}\n\' | cc \"%s\" -o \"%s\" %s -I/usr/include/skull -x c - -lm";
