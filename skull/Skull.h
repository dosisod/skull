// This code is licensed under the MIT license.
// License viewable here: https://github.com/dosisod/skull/blob/master/LICENSE

#ifndef SKULL_H
#define SKULL_H

#include <stdint.h>
#include <uchar.h>

typedef int64_t SkullInt;
typedef double SkullFloat;
typedef char32_t SkullRune;
typedef char32_t * SkullStr;

#define SKULL_INIT_MODULE(name) \
	int name(void) __asm__("." #name); name();

#endif

