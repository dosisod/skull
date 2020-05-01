#include <string.h>
#include <stdio.h>

#define LINE_COMMENT_LEN 2
const char *LINE_COMMENT="# ";

/*
Returns index of the first single line comment, starting from pointer `code`.

If no comment is found, the index of the last character is returned.
*/
long long find_line_comment(const char *code) {
	const unsigned long long code_len=strlen(code);

	const unsigned long long searchable_range=(code_len - LINE_COMMENT_LEN);
	if (searchable_range<LINE_COMMENT_LEN || searchable_range>code_len) return code_len;

	unsigned long long i=0;
	char cache[LINE_COMMENT_LEN + 1];

	for (; (i<=searchable_range); i++) {
		strncpy(cache, code+i, LINE_COMMENT_LEN);
		cache[LINE_COMMENT_LEN]='\0';
		if (strcmp(cache, LINE_COMMENT)==0) break;
	}

	return i;
}