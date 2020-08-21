#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/common/io.h"

#define IO_BUFFER_LEN 1024

/*
Returns pointer to UTF-32 string read from file descriptor `fd`.
*/
char32_t *read_file(FILE *fd, bool is_repl) {
	char *str = malloc(IO_BUFFER_LEN);
	DIE_IF_MALLOC_FAILS(str);

	size_t offset = 0;
	int c = getc(fd);

	while ((!is_repl || (c != '\n')) && c != EOF) {
		//read char by char until we need to reallocate more memory
		if (offset != 0 && ((offset + 1) % IO_BUFFER_LEN) == 0) {
			char *new_str = realloc(str, offset + IO_BUFFER_LEN);
			DIE_IF_MALLOC_FAILS(new_str);

			str = new_str;
		}
		str[offset] = (char)c;
		offset++;

		c = getc(fd);
	}
	str[offset] = '\0';

	char32_t *ret = mbstoc32s(str);

	free(str);
	return ret;
}

#undef IO_BUFFER_LEN