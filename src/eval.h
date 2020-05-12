#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

#include "../src/functions/run.h"

void eval(FILE *f) {
	fseek(f, 0, SEEK_END);
	long int size=ftell(f);
	rewind(f);

	char code[size + 1];
	fread(code, 1, size, f);
	code[size]='\0';

	char *open_bracket=strchr(code, '[');
	char *close_bracket=strchr(code, ']');
	if (open_bracket==NULL || close_bracket==NULL) {
		wprintf(L"no function found\n");
		exit(1);
	}

	size_t start_index=(size_t)(open_bracket - code);
	size_t end_index=(size_t)(close_bracket - code);
	size_t bracket_data_len=end_index - start_index;

	char bracket_data[bracket_data_len + 1];
	strncpy(bracket_data, (code + start_index + 1), bracket_data_len - 1);
	bracket_data[bracket_data_len]='\0';

	char function_name[start_index + 1];
	strncpy(function_name, code, start_index);
	function_name[start_index]='\0';

	run_function(function_name, bracket_data);
}
