#include <stdint.h>
#include <uchar.h>


static int64_t a = 1;
static const int64_t b = 2;
int64_t f(void);
static void f2(void) {
	(void)0;
}
static int64_t x;
static const int64_t int_ = 1;
static const bool bool_ = 1;
static const double float_ = 0x1.91eb851eb851fp+1;
static const char32_t rune_ = 0x78;
static const char* str_ = "hello world";
static const int64_t* int_ptr = &int_;
static const bool* bool_ptr = &bool_;
static const double* float_ptr = &float_;
static const char32_t* rune_ptr = &rune_;
static const char** str_ptr = &str_;

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
	f();
	x = (1 + f());
	return a;
	return 0;
}
int main(void) { return init(); }
