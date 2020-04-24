#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../src/functions/run.h"

void eval(FILE *f) {
	fseek(f, 0, SEEK_END);
	long size=ftell(f);
	rewind(f);

	char *code=(char*)malloc(size + 1);
	if (code==NULL) {
		printf("could not allocate enough memory\n");
		exit(2);
	}
	fread(code, 1, size, f);
	code[size]='\0';

	char *open_bracket=strchr(code, '[');
	char *close_bracket=strchr(code, ']');
	if (open_bracket==NULL || close_bracket==NULL) {
		printf("no function found\n");
		exit(1);
	}

	int start_index=(int)(open_bracket - code);
	int end_index=(int)(close_bracket - code);
	int bracket_data_len=end_index - start_index;

	char *bracket_data=(char*)malloc(bracket_data_len);
	if (bracket_data==NULL) {
		printf("could not allocate enough memory\n");
		exit(2);
	}
	strncpy(bracket_data, (code + start_index + 1), bracket_data_len - 1);

	char *function_name=(char*)malloc(start_index);
	strncpy(function_name, code, start_index);

	run_function(function_name, bracket_data);

	free(bracket_data);
	free(function_name);
}
