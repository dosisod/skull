#pragma once

#include <stdbool.h>
#include <wchar.h>
#include <wctype.h>

/*
Returns true if `wc` matches a wegex group or a single character.

If char at `begin` is `'\n'`, then return whether `wc` is a number or not.
If char at `begin` is `'\b'`, then return whether `wc` is a hexadecimal number (byte) or not.
If char at `begin` is `'\a'`, then return whether `wc` is an ASCII alpha character: `A-Z`, `a-z`
If char at `begin` is `'\f'`, then return whether `wc` is an alphanumeric character: `0-9`, `A-Z`, `a-z`, or locale specific alphanumeric character.

If char at `begin` is `'['`, then return wether `wc` matches any character within `begin` and `end`.

Else, return wether `wc` and the char at `begin` are equal.
*/
bool wegex_wc_cmp(const wchar_t *begin, const wchar_t *end, wchar_t wc) {
	if (*begin==L'[') {
		begin++;
		while (begin!=end) {
			if (wegex_wc_cmp(begin, end, wc)) {
				return true;
			}
			begin++;
		}
		return false;
	}
	if (*begin==L'\n') {
		return iswdigit(wc);
	}
	if (*begin==L'\b') {
		return iswxdigit(wc);
	}
	if (*begin==L'\a') {
		return (
			(wc>=L'A' && wc<=L'Z') ||
			(wc>=L'a' && wc<=L'z')
		);
	}
	if (*begin==L'\f') {
		return iswalnum(wc);
	}
	return *begin==wc;
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

		else if (wegex_wc_cmp(wegex, wegex_end, *match)) {
			match++;
			wegex_end=find_next_wegex(wegex);
		}
		else {
			return false;
		}

		wegex=wegex_end + 1;
	}

	while (*wegex==L'?' || *wegex==L'*') {
		const wchar_t *tmp=find_next_wegex(wegex + 1) + 1;
		if (tmp!=wegex) {
			wegex=tmp;
		}
		else {
			wegex+=2;
		}
	}

	return *wegex==*match;
}