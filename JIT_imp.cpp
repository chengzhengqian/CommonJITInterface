#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#define LLVMDebug 1
namespace llvm {


namespace orc {
class JITMemoryManager : public SectionMemoryManager {
public:
  std::map<long, long> *table;
  JITMemoryManager(std::map<long, long> *t) {
    if(LLVMDebug)
      std::cout<<"JIT memory manager!\n";
    table = t;
  }
  uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
                               unsigned SectionID,
                               StringRef SectionName) override {
    auto ptr = SectionMemoryManager::allocateCodeSection(
        Size, Alignment, SectionID, SectionName);
    if(LLVMDebug)
      std::cout<<"allocated code with size "<<Size<<" at "<<(long)ptr<<"\n";
    (*table)[(long)ptr] = (long)Size;
    return ptr;
  }
  uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
                               unsigned SectionID, StringRef SectionName,
                               bool IsReadOnly) override {
    auto ptr = SectionMemoryManager::allocateDataSection(
        Size, Alignment, SectionID, SectionName, IsReadOnly);
    if(LLVMDebug)
      std::cout<<"allocated data with size "<<Size<<" at "<<(long)ptr<<"\n";
    (*table)[(long)ptr] = (long)Size;
    return ptr;
  }
  bool finalizeMemory(std::string *ErrMsg = nullptr) override {
    return SectionMemoryManager::finalizeMemory(ErrMsg);
  }
};

class KaleidoscopeJIT {
private:
  std::unique_ptr<TargetMachine> TM;
  const DataLayout DL;
  RTDyldObjectLinkingLayer ObjectLayer;
  IRCompileLayer<decltype(ObjectLayer), SimpleCompiler> CompileLayer;

public:
  using ModuleHandle = decltype(CompileLayer)::ModuleHandleT;

  KaleidoscopeJIT(std::map<long, long> * t)
      : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
        ObjectLayer([t]() { return std::make_shared<JITMemoryManager>(t); }),
        CompileLayer(ObjectLayer, SimpleCompiler(*TM)) {
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  TargetMachine &getTargetMachine() { return *TM; }

  ModuleHandle addModule(std::unique_ptr<Module> M) {
    // Build our symbol resolver:
    // Lambda 1: Look back into the JIT itself to find symbols that are part of
    //           the same "logical dylib".
    // Lambda 2: Search for external symbols in the host process.
    auto Resolver = createLambdaResolver(
        [&](const std::string &Name) {
          if (auto Sym = CompileLayer.findSymbol(Name, false))
            return Sym;
          return JITSymbol(nullptr);
        },
        [](const std::string &Name) {
          if (auto SymAddr =
                  RTDyldMemoryManager::getSymbolAddressInProcess(Name))
            return JITSymbol(SymAddr, JITSymbolFlags::Exported);
          return JITSymbol(nullptr);
        });

    // Add the set to the JIT with the resolver we created above and a newly
    // created SectionMemoryManager.
    return cantFail(CompileLayer.addModule(std::move(M), std::move(Resolver)));
  }

  JITSymbol findSymbol(const std::string Name) {
    std::string MangledName;
    raw_string_ostream MangledNameStream(MangledName);
    Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    return CompileLayer.findSymbol(MangledNameStream.str(), true);
  }

  void removeModule(ModuleHandle H) { cantFail(CompileLayer.removeModule(H)); }
};

} // end namespace orc
} // end namespace llvm
// #include <map>
