#include <uchar.h>

/*
Returns pointer to the start of a single line comment in `code`.

If no comment is found, a pointer to the last character is returned.
*/
const char32_t *find_line_comment(const char32_t *code);