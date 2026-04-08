#include <sys/syscall.h>
#include <unistd.h>
#include "roots.h"

//gia sigouria kanoume define
#define __NR_find_roots 463

int find_roots_wrapper(void) {
	return ( syscall (__NR_find_roots) );
}
