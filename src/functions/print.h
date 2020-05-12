#include <string.h>
#include <wchar.h>

/*
Print the data passed in.

Currently, only string data (data wrapped in `"`s) will be printed.
*/
void print(const char *data) {
	size_t len=strlen(data);
	if (data[0]=='\"' && data[len - 1]=='\"') {
		wprintf(L"%.*s", len - 2, data + 1);
	}
}
