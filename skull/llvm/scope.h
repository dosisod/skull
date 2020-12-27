/*
Make new scope and set the current scope to be a sub-scope of the new one.
*/
#define MAKE_SUB_SCOPE \
	Scope *scope_copy = SCOPE; \
	SCOPE = make_scope(); \
	SCOPE->sub_scope = scope_copy

/*
Free the new scope, set the current scope to the old sub-scope.
*/
#define RESTORE_SUB_SCOPE \
	free_scope(SCOPE); \
	SCOPE = scope_copy; \
	SCOPE->sub_scope = NULL

