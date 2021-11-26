#include <stdint.h>
#include <uchar.h>


static int64_t a = 1;
static const int64_t b = 2;

static int init(void) __asm__(".first");
static int init(void) {
	a = 2;
	(void)0;
	if (1) {
		int64_t c = 3;
		const int64_t d = 4;
	}
	else if (1) {
		(void)0;
	}
	else {
		while (1) {}
	}
	while (0) {
		(void)0;
	}
	return a;
	return 0;
}
int main(void) { return init(); }
