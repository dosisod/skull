#include <stdio.h>

/*
An example of a function that will be called via `dll_run`.
*/
void hello(void) {
	puts("hello from libhello!");
}