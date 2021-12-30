#include <stdio.h>

static void gen_c_str(FILE *in, FILE *out) {
	for (;;) {
		int c = fgetc(in);

		if (c == EOF) break;

		fprintf(out, c >= 128 ? "'\\x%x', " : "%i, ", c);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage: %s file.sk file.ll file.sk.ll\n", argv[0]);
		return 1;
	}

	FILE *sk_file = fopen(argv[1], "rbe");
	FILE *ll_file = fopen(argv[2], "rbe");
	FILE *out_file = fopen(argv[3], "wb+e");

	fprintf(out_file, "pass |= e2e_wrapper(\n(char[]){ ");
	gen_c_str(sk_file, out_file);
	fprintf(out_file, "0 },\n");

	fprintf(out_file, "\"%s\",\n", argv[1]);

	fprintf(out_file, "(char[]){ ");
	gen_c_str(ll_file, out_file);
	fprintf(out_file, "0 },\nNULL\n);\n\n");
}
