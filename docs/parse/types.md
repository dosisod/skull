# src/parse/types.h

#### `bool make_new_type(const char *name)`
Returns true if the type `name` was inserted.

Returns false if a type called `name` already exists, and was not inserted.

#### `void free_types()`
Free all defined types from `TYPES_AVAILABLE`, except for `TYPES_AVAILABLE`.

#### `void make_default_types()`
Populate `TYPES_AVAILABLE` with all the builtin types.

Calling this function will reset all types defined in `TYPES_AVAILABLE`.

