#include <wchar.h>

#include "../../src/common/str.h"

#define LINE_COMMENT_LEN 2
const wchar_t *LINE_COMMENT=L"# ";

/*
Returns pointer to the start of a single line comment in `code`.

If no comment is found, a pointer to the last character is returned.
*/
const wchar_t *find_line_comment(const wchar_t *code) {
	const size_t code_len=wcslen(code);

	const size_t searchable_range=(code_len - LINE_COMMENT_LEN);
	if (searchable_range<LINE_COMMENT_LEN || searchable_range>code_len) {
		return code + code_len;
	}

	while (*code!=L'\0') {
		if (wcsncmp(LINE_COMMENT, code, LINE_COMMENT_LEN)==0) {
			return code;
		}
		code++;
	}

	return code - 1;
}