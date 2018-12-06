/* This file was automatically generated.  Do not edit! */
#undef INTERFACE
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "container.h"
LLVMExecutionEngineRef createMCJIT(LLVMModuleRef mod,MapRef mr);
void destroyManager(void *opaque);
LLVMBool finalizeMemory(void *opaque,char **msg);
uint8_t *dataCallback(void *opaque,uintptr_t size,unsigned alignmemt,unsigned id,const char *sectionName,LLVMBool IsReadOnly);
uint8_t *codeCallback(void *opaque,uintptr_t size,unsigned alignmemt,unsigned id,const char *sectionName);
#define INTERFACE 0
