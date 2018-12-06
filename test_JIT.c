#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/ExecutionEngine.h>
#include <sys/mman.h> //for mmap
#include <stdio.h>
#include "JIT.h"
typedef JIT *JITRef;

void printModule(LLVMModuleRef mod){
  const char *s=LLVMPrintModuleToString(mod);
  printf("%s\n",s);
  LLVMDisposeMessage(s);
}

int main(){
  LLVMInitializeAllTargets();
  LLVMInitializeAllTargetMCs();
  LLVMInitializeAllAsmPrinters();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeTarget();
  
  LLVMContextRef ctx=LLVMGetGlobalContext();
  
  LLVMModuleRef mod=LLVMModuleCreateWithNameInContext("czq",ctx);
  LLVMTypeRef doubleType=LLVMDoubleTypeInContext(ctx);
  LLVMTypeRef argsType[2]={doubleType,doubleType};
  LLVMTypeRef funcType=LLVMFunctionType(doubleType,argsType,2,0);
  LLVMValueRef func=LLVMAddFunction(mod,"func",funcType);
  /* printf("Add Function:\n %s\n",LLVMPrintModuleToString(mod)); */
  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry:");

  
  LLVMValueRef params[2];
  LLVMGetParams(func,params);
  LLVMBuilderRef builder=LLVMCreateBuilderInContext(ctx);
  LLVMPositionBuilder(builder,block,0);
  LLVMValueRef add=LLVMBuildFAdd(builder,params[0],params[1],"add");
  LLVMValueRef const_2=LLVMConstReal(doubleType,2.0);
  LLVMValueRef add2=LLVMBuildFAdd(builder,const_2,add,"add2");
  LLVMBuildRet(builder,add2);

  printf("First:\n"); printModule(mod);
  JITRef jit= createJIT();
  addModule(jit,mod);
  /* printf("%s",*error); */
  uint64_t func_=findSymbol(jit,"func");
  printf("%ld\n",func_);
  printf("Call func with %f,%f and get %f\n",1.0,2.0,((double (*)(double,double))func_)(1.0,2.0));

  //Now, we create the second module
  LLVMModuleRef mod2=LLVMModuleCreateWithNameInContext("czq",ctx);
  LLVMTypeRef argsType2[0]={};
  LLVMTypeRef funcType2=LLVMFunctionType(doubleType,argsType2,0,0);
  LLVMValueRef func2=LLVMAddFunction(mod2,"func2",funcType2);
  LLVMBasicBlockRef block2=LLVMAppendBasicBlock(func2,"entry:");
  LLVMPositionBuilder(builder,block2,0);
  LLVMValueRef call_params[2]={const_2,const_2};
  func=LLVMAddFunction(mod2,"func",funcType);
  LLVMValueRef call_ret=LLVMBuildCall(builder,func,call_params,2,"call_fun");
  /* LLVMBuildRet(builder,const_2); */
  LLVMBuildRet(builder,call_ret);
  printf("Add Instruction:\n"); printModule(mod2);
  //the orc is more aggressive, in mc, one can refer the function after.. but for.. we can't
  addModule(jit,mod2);
  uint64_t func2_=findSymbol(jit,"func2");
  printf("%ld\n",func2_);
  printf("Call func2 and get %f\n",((double (*)())func2_)());
  
  /* LLVMDisposeExecutionEngine(jit); */
  /* LLVMDisposeBuilder(builder); */
  /* /\*it seems that we don't need to dispose them, it is similar in llvm*\/ */
  /* /\* LLVMDisposeMCJITMemoryManager(mm); *\/ */
  /* /\* LLVMDisposeModule(mod); *\/ */
  /* LLVMContextDispose(ctx); */
  return 0;
}

