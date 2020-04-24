#include <string.h>
#include <stdio.h>

void print(const char *data) {
	int len=strlen(data);
	if (data[0]=='\"' && data[len - 1]=='\"') {
		printf("%.*s", len - 2, data + 1);
	}
}
