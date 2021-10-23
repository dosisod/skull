#pragma once

typedef struct {
	const char *filename;
	char *out_file;
	_Bool quiet;
	_Bool c_backend;
	_Bool asm_backend;
	_Bool debug;
	_Bool preprocess;
	_Bool compile_only;
	_Bool werror;
} BuildData;

extern BuildData BUILD_DATA;
