#pragma once

typedef struct {
	const char *filename;
	_Bool c_backend : 1;
	_Bool debug : 1;
} BuildData;

extern BuildData BUILD_DATA;
