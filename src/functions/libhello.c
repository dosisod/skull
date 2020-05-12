#include <wchar.h>

/*
An example of a function that will be called via `dll_run`.
*/
void hello() {
	wprintf(L"hello from libhello!\n");
}