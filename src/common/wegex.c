#include <stddef.h>

#include "str.h"

#include "wegex.h"

bool wegex_cmp(const char32_t *begin, const char32_t *end, char32_t c) {
	if (*begin==U'[') {
		begin++;
		while (begin!=end) {
			if (wegex_cmp(begin, end, c)) {
				return true;
			}
			begin++;
		}
		return false;
	}
	if (*begin==U'\n') {
		return c32isdigit(c);
	}
	if (*begin==U'\b') {
		return c32isxdigit(c);
	}
	if (*begin==U'\a') {
		return (
			(c>=U'A' && c<=U'Z') ||
			(c>=U'a' && c<=U'z')
		);
	}
	if (*begin==U'\f') {
		return c32isalnum(c);
	}
	return *begin==c;
}

const char32_t *find_next_wegex(const char32_t* wegex) {
	if (*wegex!=U'[') {
		return wegex;
	}

	const char32_t *bracket=c32schr(wegex, U']');

	if (bracket!=NULL) {
		return bracket;
	}

	return wegex;
}

bool wegex_match(const char32_t *wegex, const char32_t *match) {
	const char32_t *wegex_end=wegex;

	while (*wegex!=U'\0' && *match!=U'\0') {
		if (*wegex==U'*') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			while (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex==U'+') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			if (!wegex_cmp(wegex, wegex_end, *match)) {
				return false;
			}

			match++;
			while (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex==U'?') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			if (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (wegex_cmp(wegex, wegex_end, *match)) {
			match++;
			wegex_end=find_next_wegex(wegex);
		}
		else {
			return false;
		}

		wegex=wegex_end + 1;
	}

	while (*wegex==U'?' || *wegex==U'*') {
		const char32_t *tmp=find_next_wegex(wegex + 1) + 1;
		if (tmp!=wegex) {
			wegex=tmp;
		}
		else {
			wegex+=2;
		}
	}

	return *wegex==*match;
}