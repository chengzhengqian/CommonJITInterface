#if INTERFACE
#include <stdio.h>
#include <llvm-c/Core.h>
#endif

#include "missing.h"

void LLVMDumpType(LLVMTypeRef Val){
  char *s=LLVMPrintTypeToString(Val);
  printf("%s\n",s);
  LLVMDisposeMessage(s);
}
