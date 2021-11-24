#include <stdint.h>
#include <uchar.h>

static int init(void) __asm__(".first");
static int init(void) {
	int64_t x = 1;
	x = 2;
	(void)0;
	if (1) {
		(void)0;
	}
	else if (1) {
		(void)0;
	}
	else {
		(void)0;
	}
	while (0) {
		(void)0;
	}
	return x;
	return 0;
}
int main(void) { return init(); }
