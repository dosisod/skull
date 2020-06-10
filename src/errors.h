#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define ERROR_INVALID_INPUT 0
#define ERROR_VAR_ALREADY_DEFINED 1
#define ERROR_CANNOT_ASSIGN_CONST 2
#define ERROR_TYPE_MISMATCH 3
#define ERROR_OVERFLOW 4

const wchar_t *ERROR_MSG[] = {
	L"invalid input",
	L"variable already defined",
	L"cannot assign to const",
	L"error, type mismatch",
	L"overflow occurred",
	L""
};
const wchar_t **ERROR_MSG_LAST=NULL;

/*
Returns if a given string is an error msg.
*/
bool is_error_msg(const wchar_t *str) {
	if (str==NULL) {
		return false;
	}

	if (ERROR_MSG_LAST==NULL) {
		const wchar_t **head=ERROR_MSG;

		while (*head[0]!=L'\0') {
			ERROR_MSG_LAST=head;
			head++;
		}
	}

	return (ERROR_MSG_LAST!=NULL) && (str <= *ERROR_MSG_LAST) && (str >= ERROR_MSG[0]);
}