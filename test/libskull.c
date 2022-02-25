#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>

int main(void) {
	errno = 0;
	void *handle = dlopen("libskull.so", RTLD_LAZY);
	if (errno) {
		perror("dlopen");
		return 1;
	}

	int (*skull_main)(int, char *[]) = dlsym(handle, "main");
	if (errno) {
		perror("dlsym");
		return 1;
	}

	skull_main(2, (char *[]){"skull", "-v"});
}
