#ifndef JIT_H
#define JIT_H

#include <llvm-c/Types.h>
typedef struct JIT* JITRef;
typedef struct Handle *HandleRef;
JITRef createJIT();
void disposeJIT(JITRef jit);
HandleRef addModule(JITRef jit, LLVMModuleRef m);
void removeModule(JITRef jit, HandleRef h);
long findSymbol(JITRef jit, const char *s);
// long getSymbolSize(JITRef jit,long addr);
long getSectionSize(JITRef jit,long addr);
long getSectionStart(JITRef jit, long addr);
void printASM(long start, long size) ;


#endif


