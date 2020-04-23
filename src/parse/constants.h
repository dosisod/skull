#include <string.h>
#include <stdio.h>

const char *LINE_COMMENT="# ";
const short LINE_COMMENT_LEN=2;

long long find_line_comment(const char *code) {
	const long long searchable_range=(strlen(code) - LINE_COMMENT_LEN);
	if (searchable_range<LINE_COMMENT_LEN) return strlen(code);

	unsigned long long i=0;
	char cache[LINE_COMMENT_LEN + 1];

	for (; (i<=searchable_range); i++) {
		strncpy(cache, code+i, LINE_COMMENT_LEN);
		cache[LINE_COMMENT_LEN]='\0';
		if (strcmp(cache, LINE_COMMENT)==0) break;
	}

	return i;
}