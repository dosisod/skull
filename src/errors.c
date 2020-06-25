#include <stddef.h>

#include "errors.h"

const char32_t *ERROR_MSG[] = {
	U"invalid input",
	U"variable already defined",
	U"cannot assign to const",
	U"error, type mismatch",
	U"overflow occurred",
	U""
};
const char32_t **ERROR_MSG_LAST=NULL;

bool is_error_msg(const char32_t *str) {
	if (str==NULL) {
		return false;
	}

	if (ERROR_MSG_LAST==NULL) {
		const char32_t **head=ERROR_MSG;

		while (*head[0]!=U'\0') {
			ERROR_MSG_LAST=head;
			head++;
		}
	}

	return (ERROR_MSG_LAST!=NULL) && (str <= *ERROR_MSG_LAST) && (str >= ERROR_MSG[0]);
}