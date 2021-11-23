#include <stdint.h>
#include <uchar.h>

static int init(void) __asm__(".first");
static int init(void) {
	int64_t x = 1;
	x = 2;
	(void)0;
	return x;
	return 0;
}
int main(void) { return init(); }
