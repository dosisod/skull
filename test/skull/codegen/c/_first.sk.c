_Bool _strcmp(const char *a, const char *b) {
	while (*a && *b) {
		if (*a != *b) {
			return 0;
		}
		a++;
		b++;
	}

	return !*a && !*b;
}

#include <math.h>
double (*_float_pow)(double, double) = pow;

#include <stdint.h>
int64_t _int_pow(int64_t base, int64_t exp) {
	int64_t result = base;
	for (int64_t i = 1; i < exp; i++) result *= base;
	return result;
}

static int init(void) __asm__("./test/skull/codegen/c/.first");
static int init(void) { return 0; }
int main(void) { return init(); }
