#if INTERFACE
#include <stdlib.h>
#include <stdint.h>

#endif

#include "runFunc.h"

void runFunc(uint64_t pointer) { ((void (*)())pointer)(); }
double  runFuncDouble(uint64_t pointer) { return ((double (*)())pointer)(); }


