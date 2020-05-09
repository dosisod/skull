#pragma once

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "../common/str.h"

/*
Run a void(void) function pointer from a dll.

If the dll is not found, or symbol was not found, function does nothing.
Else, the function is invoked.
*/
void dll_run(const char *data) {
	char *comma=strstr(data, "\", \"");
	if (comma==NULL) {
		printf("dll_run expects 2 params, got 1\n");
		return;
	}

	int param1_len=(comma - data - 1);
	char param1[param1_len + 1];
	strlcpy(param1, data + 1, param1_len);

	int param2_len=(strlen(data) - param1_len - 6);
	char param2[param2_len + 1];
	strlcpy(param2, comma + 4, param2_len);

	void *handle=dlopen(param1, RTLD_NOW);
	if (handle==NULL) {
		printf("\"%s\" could not be loaded\n", param1);
		return;
	}

	void(*callable)();
	*(void **)(&callable)=dlsym(handle, param2);
	if (callable==NULL) {
		printf("could not call \"%s\" on \"%s\"\n", param2, param1);
		return;
	}

	callable();
}
