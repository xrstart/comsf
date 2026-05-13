#include <stdlib.h>

void __stack_chk_fail(void) { abort(); }
uintptr_t __stack_chk_guard = 0x595e9f595e9f595eULL;
