#include <stdio.h>

#include "../src/eval.h"

int main() {
	FILE *f;
	f=fopen("./main.skull", "r");

	if (f==NULL) {
		printf("\"main.skull\" was not found, exiting\n");
		return 1;
	}

	eval(f);
	fclose(f);

	return 0;
}