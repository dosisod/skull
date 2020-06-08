#include <stdbool.h>

#include "../../src/common/color.h"
#include "../../test/testing.h"

TEST(color_names, {
	return (
		wcscmp(L"\033[0m", COLOR_RESET)==0 &&
		wcscmp(L"\033[1m", COLOR_BOLD)==0 &&
		wcscmp(L"\033[4m", COLOR_UNDERLINE)==0 &&
		wcscmp(L"\033[7m", COLOR_INVERT)==0 &&
		wcscmp(L"\033[9m", COLOR_STRIKE)==0 &&

		wcscmp(L"\033[30m", COLOR_GRAY_FG)==0 &&
		wcscmp(L"\033[31m", COLOR_RED_FG)==0 &&
		wcscmp(L"\033[32m", COLOR_GREEN_FG)==0 &&
		wcscmp(L"\033[33m", COLOR_YELLOW_FG)==0 &&
		wcscmp(L"\033[34m", COLOR_BLUE_FG)==0 &&
		wcscmp(L"\033[35m", COLOR_PURPLE_FG)==0 &&
		wcscmp(L"\033[36m", COLOR_TEAL_FG)==0 &&
		wcscmp(L"\033[37m", COLOR_WHITE_FG)==0 &&

		wcscmp(L"\033[40m", COLOR_GRAY_BG)==0 &&
		wcscmp(L"\033[41m", COLOR_RED_BG)==0 &&
		wcscmp(L"\033[42m", COLOR_GREEN_BG)==0 &&
		wcscmp(L"\033[43m", COLOR_YELLOW_BG)==0 &&
		wcscmp(L"\033[44m", COLOR_BLUE_BG)==0 &&
		wcscmp(L"\033[45m", COLOR_PURPLE_BG)==0 &&
		wcscmp(L"\033[46m", COLOR_TEAL_BG)==0 &&
		wcscmp(L"\033[47m", COLOR_WHITE_BG)==0 &&

		wcscmp(L"\033[90m", COLOR_BRIGHT_GRAY_FG)==0 &&
		wcscmp(L"\033[91m", COLOR_BRIGHT_RED_FG)==0 &&
		wcscmp(L"\033[92m", COLOR_BRIGHT_GREEN_FG)==0 &&
		wcscmp(L"\033[93m", COLOR_BRIGHT_YELLOW_FG)==0 &&
		wcscmp(L"\033[94m", COLOR_BRIGHT_BLUE_FG)==0 &&
		wcscmp(L"\033[95m", COLOR_BRIGHT_PURPLE_FG)==0 &&
		wcscmp(L"\033[96m", COLOR_BRIGHT_TEAL_FG)==0 &&
		wcscmp(L"\033[97m", COLOR_BRIGHT_WHITE_FG)==0 &&

		wcscmp(L"\033[100m", COLOR_BRIGHT_GRAY_BG)==0 &&
		wcscmp(L"\033[101m", COLOR_BRIGHT_RED_BG)==0 &&
		wcscmp(L"\033[102m", COLOR_BRIGHT_GREEN_BG)==0 &&
		wcscmp(L"\033[103m", COLOR_BRIGHT_YELLOW_BG)==0 &&
		wcscmp(L"\033[104m", COLOR_BRIGHT_BLUE_BG)==0 &&
		wcscmp(L"\033[105m", COLOR_BRIGHT_PURPLE_BG)==0 &&
		wcscmp(L"\033[106m", COLOR_BRIGHT_TEAL_BG)==0 &&
		wcscmp(L"\033[107m", COLOR_BRIGHT_WHITE_BG)==0
	);
});

void color_test_self(bool *pass) {
	tests_t tests={
		test_color_names,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
