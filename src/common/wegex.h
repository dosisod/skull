#pragma once

#include <stdbool.h>
#include <wchar.h>
#include <wctype.h>

/*
Returns true if `wc` matches a wegex group or a single character.

If char at `start` is `'\n'`, then return wether `wc` is a number or not.
If char at `start` is `'\b'`, then return wether `wc` is a hexadecimal number (byte) or not.

If char at `start` is `'['`, then return wether `wc` matches any character within `start` and `end`.

Else, return wether `wc` and the char at `start` are equal.
*/
bool wegex_wc_cmp(const wchar_t *start, const wchar_t *end, wchar_t wc) {
	if (*start==L'[') {
		start++;
		while (start!=end) {
			if (wegex_wc_cmp(start, end, wc)) {
				return true;
			}
			start++;
		}
		return false;
	}
	if (*start==L'\n') {
		return iswdigit(wc);
	}
	if (*start==L'\b') {
		return iswxdigit(wc);
	}
	return *start==wc;
}

/*
Returns a pointer to the next searchable wegex group.

If `wegex` is pointing to a `'['` character, return the corresponding `']'`.
Else, return the passed wegex.
*/
const wchar_t *find_next_wegex(const wchar_t* wegex) {
	if (*wegex!=L'[') {
		return wegex;
	}

	const wchar_t *bracket=wcschr(wegex, L']');

	if (bracket!=NULL) {
		return bracket;
	}

	return wegex;
}

/*
Returns true if the `wegex` string matches the `match` string.

"wegex" is a wide-char regex implementation, and uses different syntax from regex:
* Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
* "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.
* Character groups can be used, for example, `+[123]+[abc]` matches `"321cba"`.
*/
bool wegex_match(const wchar_t *wegex, const wchar_t *match) {
	const wchar_t *wegex_end=wegex;

	while (*wegex!=L'\0' && *match!=L'\0') {
		if (*wegex==L'*') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			while (wegex_wc_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex==L'+') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			if (!wegex_wc_cmp(wegex, wegex_end, *match)) {
				return false;
			}

			match++;
			while (wegex_wc_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (*wegex==L'?') {
			wegex++;
			wegex_end=find_next_wegex(wegex);

			if (wegex_wc_cmp(wegex, wegex_end, *match)) {
				match++;
			}
		}

		else if (!wegex_wc_cmp(wegex, wegex_end, *match)) {
			return false;
		}

		else {
			match++;
			wegex_end=find_next_wegex(wegex);
		}

		wegex=wegex_end + 1;
	}

	return *wegex==*match;
}