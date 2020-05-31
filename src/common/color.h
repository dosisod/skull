#pragma once

#include <wchar.h>

#define COLOR_RESET L"\033[0m"
#define COLOR_BOLD L"\033[1m"
#define COLOR_UNDERLINE L"\033[4m"
#define COLOR_INVERT L"\033[7m"
#define COLOR_STRIKE L"\033[9m"

#define COLOR_GRAY_FG L"\033[30m"
#define COLOR_RED_FG L"\033[31m"
#define COLOR_GREEN_FG L"\033[32m"
#define COLOR_YELLOW_FG L"\033[33m"
#define COLOR_BLUE_FG L"\033[34m"
#define COLOR_PURPLE_FG L"\033[35m"
#define COLOR_TEAL_FG L"\033[36m"
#define COLOR_WHITE_FG L"\033[37m"

#define COLOR_GRAY_BG L"\033[40m"
#define COLOR_RED_BG L"\033[41m"
#define COLOR_GREEN_BG L"\033[42m"
#define COLOR_YELLOW_BG L"\033[43m"
#define COLOR_BLUE_BG L"\033[44m"
#define COLOR_PURPLE_BG L"\033[45m"
#define COLOR_TEAL_BG L"\033[46m"
#define COLOR_WHITE_BG L"\033[47m"

#define COLOR_BRIGHT_GRAY_FG L"\033[90m"
#define COLOR_BRIGHT_RED_FG L"\033[91m"
#define COLOR_BRIGHT_GREEN_FG L"\033[92m"
#define COLOR_BRIGHT_YELLOW_FG L"\033[93m"
#define COLOR_BRIGHT_BLUE_FG L"\033[94m"
#define COLOR_BRIGHT_PURPLE_FG L"\033[95m"
#define COLOR_BRIGHT_TEAL_FG L"\033[96m"
#define COLOR_BRIGHT_WHITE_FG L"\033[97m"

#define COLOR_BRIGHT_GRAY_BG L"\033[100m"
#define COLOR_BRIGHT_RED_BG L"\033[101m"
#define COLOR_BRIGHT_GREEN_BG L"\033[102m"
#define COLOR_BRIGHT_YELLOW_BG L"\033[103m"
#define COLOR_BRIGHT_BLUE_BG L"\033[104m"
#define COLOR_BRIGHT_PURPLE_BG L"\033[105m"
#define COLOR_BRIGHT_TEAL_BG L"\033[106m"
#define COLOR_BRIGHT_WHITE_BG L"\033[107m"

/*
Reset current colors to terminal defaults.
*/
void color_reset(void) {
	wprintf(COLOR_RESET);
}