#include <stdlib.h>

#include "skull/build_data.h"
#include "skull/codegen/c/debug.h"

#include "skull/common/str.h"

char *apply_debug_info(char *str, const AstNode *node) {
	if (!BUILD_DATA.debug) return str;

	char *old = str;
	str = uvsnprintf(
		"#line %li \"%s\"\n%s",
		node->token->location.line,
		BUILD_DATA.filename,
		old
	);
	free(old);

	return str;
}
