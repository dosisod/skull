# src/functions/dll.h

#### `void dll_run(const char *data)`
Run a void(void) function pointer from a dll.

If the dll is not found, or symbol was not found, function does nothing.
Else, the function is invoked.

