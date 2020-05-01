#include <string.h>
#include <stdio.h>

/*
Print the data passed in.

Currently, only string data (data wrapped in `"`s) will be printed.
*/
void print(const char *data) {
	int len=strlen(data);
	if (data[0]=='\"' && data[len - 1]=='\"') {
		printf("%.*s", len - 2, data + 1);
	}
}
