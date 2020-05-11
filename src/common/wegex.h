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
	while (*wegex!=L'\0' && *match!=L'\0') {
		if (*wegex==L'*') {
			wegex++;
			while (wegex_wc_cmp(*wegex, *match)) {
				match++;
			}
			wegex++;
		}

		else if (*wegex==L'+') {
			wegex++;
			if (!wegex_wc_cmp(*wegex, *match)) {
				return false;
			}
			match++;
			while (wegex_wc_cmp(*wegex, *match)) {
				match++;
			}
			wegex++;
		}

		else if (*wegex==L'?') {
			wegex++;
			if (wegex_wc_cmp(*wegex, *match)) {
				match++;
			}

			wegex++;
		}

		else if (!wegex_wc_cmp(*wegex, *match)) {
			return false;
		}

		else {
			wegex++;
			match++;
		}
	}

	return *wegex==*match;
}