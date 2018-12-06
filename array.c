#if INTERFACE
#include <stdlib.h>
#include <llvm-c/Types.h>
#endif

#include "array.h"

void **createPtrArray(int n) {
  void **array = malloc(n * sizeof(void *));
  return array;
}

void disposePtrArray(void **array) { free(array); }

void setPtrArrayAt(void** array, int n, void *element) {
  array[n]=element;
}

void* getPtrArrayAt(void** array,int n){
  return array[n];
}


LLVMTypeRef* toTypeArray(void** array){
  return (LLVMTypeRef*) array;
}

LLVMValueRef* toValueArray(void** array){
  return (LLVMValueRef*) array;
}

LLVMValueRef toValue(void* element){
  return (LLVMValueRef)element;
}

LLVMTypeRef toType(void* element){
  return (LLVMTypeRef)element;
}
