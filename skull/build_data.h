#pragma once

typedef struct {
	const char *filename;
	char *out_file;
	_Bool quiet : 1;
	_Bool c_backend : 1;
	_Bool asm_backend : 1;
	_Bool debug : 1;
	_Bool preprocess : 1;
	_Bool compile_only : 1;
	_Bool werror : 1;
} BuildData;

extern BuildData BUILD_DATA;
