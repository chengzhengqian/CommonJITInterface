/* This file was automatically generated.  Do not edit! */
#undef INTERFACE
#include <stdlib.h>
#include <llvm-c/Types.h>
LLVMTypeRef toType(void *element);
LLVMValueRef toValue(void *element);
LLVMValueRef *toValueArray(void **array);
LLVMTypeRef *toTypeArray(void **array);
void *getPtrArrayAt(void **array,int n);
void setPtrArrayAt(void **array,int n,void *element);
void disposePtrArray(void **array);
void **createPtrArray(int n);
#define INTERFACE 0
