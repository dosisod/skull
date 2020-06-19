#include <stdio.h>
#include <string.h>

/*
Print the data passed in.

Currently, only string data (data wrapped in `"`s) will be printed.
*/
void print(const char *data) {
	const size_t len=strlen(data);
	if (data[0]=='\"' && data[len - 1]=='\"') {
		printf("%.*s", (int)(len - 2), data + 1);
	}
}
