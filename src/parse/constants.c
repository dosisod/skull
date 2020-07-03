#include "../../src/common/str.h"

#include "constants.h"

const char32_t *LINE_COMMENT=U"# ";

/*
Returns pointer to the start of a single line comment in `code`.

If no comment is found, a pointer to the last character is returned.
*/
const char32_t *find_line_comment(const char32_t *code) {
	const size_t code_len=c32slen(code);

	const size_t searchable_range=(code_len - LINE_COMMENT_LEN);
	if (searchable_range<LINE_COMMENT_LEN || searchable_range>code_len) {
		return code + code_len;
	}

	while (*code!=U'\0') {
		if (c32sncmp(LINE_COMMENT, code, LINE_COMMENT_LEN)) {
			return code;
		}
		code++;
	}

	return code - 1;
}