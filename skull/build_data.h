#pragma once

typedef struct {
	const char *filename;
	const char *error_file;
	_Bool c_backend : 1;
	_Bool debug : 1;
} BuildData;

extern BuildData BUILD_DATA;

void setup_flags(void);
