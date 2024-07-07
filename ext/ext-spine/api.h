#if !CODE

#include "spine/spine.h"

#else

#pragma comment(lib, "spine-mt")

AUTORUN {
	puts("spine runtime installed: " __FILE__);
}

// make -Iext\ext-spine\runtime

#endif
