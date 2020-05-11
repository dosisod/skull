#include <stdbool.h>
#include <wchar.h>
#include <wctype.h>

/*
Returns true if `control` is comparable to `wc`.

If `control` is `'\n'`, then return wether `wc` is a number or not.
If `control` is `'\b'`, then return wether `wc` is a hexadecimal nuber (byte) or not.

Else, return wether `control` and `wc` are equal.
*/
bool wegex_wc_cmp(wchar_t control, wchar_t wc) {
	if (control==L'\n') {
		return iswdigit(wc);
	}
	if (control==L'\b') {
		return iswxdigit(wc);
	}
	return control==wc;
}

/*
Returns true if the `wegex` string matches the `match` string.

"wegex" is a wide-char regex implementation, and uses different syntax from regex:
* Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
* "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.
*/
bool wegex_match(const wchar_t *wegex, const wchar_t *match) {
	size_t wegex_index=0;
	size_t match_index=0;

	wchar_t wegex_current=wegex[0];
	wchar_t match_current=match[0];

	while (wegex_current!=L'\0' && match_current!=L'\0') {
		if (wegex_current==L'*') {
			while (wegex_wc_cmp(wegex[wegex_index + 1], match_current)) {
				match_index++;
				match_current=match[match_index];
			}
			wegex_index+=2;
			wegex_current=wegex[wegex_index];
			continue;
		}

		if (wegex_current==L'+') {
			if (!wegex_wc_cmp(wegex[wegex_index + 1], match_current)) {
				return false;
			}
			while (wegex_wc_cmp(wegex[wegex_index + 1], match_current)) {
				match_index++;
				match_current=match[match_index];
			}
			wegex_index+=2;
			wegex_current=wegex[wegex_index];
			continue;
		}

		if (wegex_current==L'?') {
			if (wegex_wc_cmp(wegex[wegex_index + 1], match_current)) {
				match_index++;
				match_current=match[match_index];
			}

			wegex_index+=2;
			wegex_current=wegex[wegex_index];
			continue;
		}

		if (!wegex_wc_cmp(wegex_current, match_current)) {
			return false;
		}

		wegex_index++;
		match_index++;
		wegex_current=wegex[wegex_index];
		match_current=match[match_index];
	}

	return wegex_current==match_current;
}