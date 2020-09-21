#include <stdlib.h>

#include "skull/common/malloc.h"

#include "skull/common/io.h"

#define IO_BUFFER_LEN 1024

/*
Return string read from file descriptor `fd`.
*/
char *read_file(FILE *fd) {
	char *str = malloc(IO_BUFFER_LEN);
	DIE_IF_MALLOC_FAILS(str);

	size_t offset = 0;
	int c = getc(fd);

	while (c != EOF) {
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

	return str;
}

#undef IO_BUFFER_LEN