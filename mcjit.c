#if INTERFACE
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> //for mmap
#include "container.h"
#endif

#include "mcjit.h"
#define MCJIT_DEBUG 1
uint8_t *codeCallback(void *opaque, uintptr_t size, unsigned alignmemt,
                      unsigned id, const char *sectionName) {
  int property = PROT_READ | PROT_WRITE | PROT_EXEC;
  int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  uint8_t *page = (uint8_t *)mmap(NULL, size, property, flags, -1, 0);
  MapRef mm=(MapRef)opaque;
  setMapLL(mm,(long)page,(long)size);
  if (MCJIT_DEBUG)
    printf("code %ld with size %ld\n", (long)page, (long)size);
  return page;
}

uint8_t *dataCallback(void *opaque, uintptr_t size, unsigned alignmemt,
                      unsigned id, const char *sectionName,
                      LLVMBool IsReadOnly) {
  int property = PROT_READ | PROT_WRITE;
  int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  uint8_t *page = (uint8_t *)mmap(NULL, size, property, flags, -1, 0);
  MapRef mm=(MapRef)opaque;
  setMapLL(mm,(long)page,(long)size);
  if (MCJIT_DEBUG)
    printf("data %ld with size %ld\n", (long)page, (long)size);
  return page;
}

LLVMBool finalizeMemory(void *opaque, char **msg) {
  if (MCJIT_DEBUG)
    printf("finalized %ld\n", (long)opaque);
  return 0;
}

void destroyManager(void *opaque) {
  if (MCJIT_DEBUG)
    printf("destroy %ld\n", (long)opaque);
}

LLVMExecutionEngineRef createMCJIT(LLVMModuleRef mod , MapRef mr) {
  // FIXME: remove the mapped memory when the engine is disposed
  LLVMExecutionEngineRef jit;
  LLVMMCJITMemoryManagerRef mm = LLVMCreateSimpleMCJITMemoryManager(
      mr, codeCallback, dataCallback, finalizeMemory, destroyManager);
  struct LLVMMCJITCompilerOptions options;
  LLVMInitializeMCJITCompilerOptions(&options, sizeof(options));
  options.MCJMM = mm;
  options.OptLevel = 0;
  char **error;
  LLVMCreateMCJITCompilerForModule(&jit, mod, &options, sizeof(options), error);
  return jit;
}
