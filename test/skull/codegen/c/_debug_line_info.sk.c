#include <stdint.h>
#include <uchar.h>

void puts(char* s);
#line 3 "./test/skull/codegen/c/debug_line_info.sk"
static void print(char* s) {
#line 4 "./test/skull/codegen/c/debug_line_info.sk"
	puts(s);
}
static int64_t i = 1;

#line 1 "./test/skull/codegen/c/debug_line_info.sk"
static int init(void) __asm__(".debug_line_info");
static int init(void) {
#line 7 "./test/skull/codegen/c/debug_line_info.sk"
	print("starting");
#line 11 "./test/skull/codegen/c/debug_line_info.sk"
	while ((i < 10)) {
#line 12 "./test/skull/codegen/c/debug_line_info.sk"
		i = (i + 1);
#line 14 "./test/skull/codegen/c/debug_line_info.sk"
		print("hello world");
	}
#line 17 "./test/skull/codegen/c/debug_line_info.sk"
	print("ending");
	return 0;
}
#line 1 "./test/skull/codegen/c/debug_line_info.sk"
int main(void) { return init(); }
