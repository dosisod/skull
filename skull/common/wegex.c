#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/str.h"

#include "skull/common/wegex.h"

/*
Returns true if the `wegex` string matches the `match` string.

"wegex" is a wide-char regex implementation, and uses different syntax from regex:
* Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
* "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.
* Character groups can be used, for example, `+[123]+[abc]` matches `"321cba"`.
*/
bool wegex_match(const char *wegex, const char32_t *match) {
	const char *wegex_end = wegex;

	while (*wegex && *match) {
		if (*wegex == '*') {
			wegex++;
			wegex_end = find_next_wegex(wegex);

			while (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex == '+') {
			wegex++;
			wegex_end = find_next_wegex(wegex);

			if (!wegex_cmp(wegex, wegex_end, *match)) {
				return false;
			}

			match++;
			while (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex == '?') {
			wegex++;
			wegex_end = find_next_wegex(wegex);

			if (wegex_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (wegex_cmp(wegex, wegex_end, *match)) {
			match++;
			wegex_end = find_next_wegex(wegex);
		}
		else {
			return false;
		}

		wegex = wegex_end + 1;
	}

	while (*wegex == '?' || *wegex == '*') {
		const char *tmp = find_next_wegex(wegex + 1) + 1;
		if (tmp != wegex) {
			wegex = tmp;
		}
		else {
			wegex += 2;
		}
	}

	return (char32_t)*wegex == *match;
}

/*
Returns a pointer to the next searchable wegex group.

If `wegex` is pointing to a `'['` character, return the corresponding `']'`.
Else, return the passed wegex.
*/
const char __attribute__((pure)) *find_next_wegex(const char *wegex) {
	if (*wegex != '[') {
		return wegex;
	}

	const char *bracket = strchr(wegex, ']');

	if (bracket) {
		return bracket;
	}

	return wegex;
}

/*
Returns true if `c` matches a wegex group or a single character.

If char at `begin` is `'\n'`, then return whether `c` is a number or not.
If char at `begin` is `'\b'`, then return whether `c` is a hexadecimal number (byte) or not.
If char at `begin` is `'\a'`, then return whether `c` is an ASCII alpha character: `A-Z`, `a-z`
If char at `begin` is `'\f'`, then return whether `c` is an alphanumeric character: `0-9`, `A-Z`, `a-z`

If char at `begin` is `'['`, then return wether `c` matches any character within `begin` and `end`.

Else, return wether `c` and the char at `begin` are equal.
*/
bool wegex_cmp(const char *begin, const char *end, char32_t c) {
	if (*begin == '[') {
		begin++;
		while (begin != end) {
			if (wegex_cmp(begin, end, c)) {
				return true;
			}
			begin++;
		}
		return false;
	}
	if (*begin == '\n') {
		return c32isdigit(c);
	}
	if (*begin == '\b') {
		return c32isxdigit(c);
	}
	if (*begin == '\a') {
		return (
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z')
		);
	}
	if (*begin == '\f') {
		return c32isalnum(c);
	}
	return (char32_t)*begin == c;
}