#include "JIT_imp.cpp"
#include <fstream>
using namespace llvm::orc;
using namespace llvm;
using Handle =   typename KaleidoscopeJIT::ModuleHandle;
// #define ModuleMap std::map<Module*,Handle>

class JIT: public KaleidoscopeJIT{
public:
  std::map<long, long> map;
  JIT():KaleidoscopeJIT(&map){};
};

extern "C"{
  JIT* createJIT(){
    return new JIT();
  }
  void disposeJIT(JIT* jit){
    delete jit;
  }
  Handle* addModule(JIT* jit,Module* m){
    // Handle h= jit->addModule(std::move(std::unique_ptr<Module>(m)));
    // (jit->map)[m]=h;
    Handle* new_h=new Handle();
    (*new_h)=jit->addModule(std::move(std::unique_ptr<Module>(m)));
    return new_h;
  }
  void removeModule(JIT* jit,Handle* h){
    // jit->removeModule((jit->map)[m]);
    // (jit->map).erase(m);
    jit->removeModule(*h);
    delete h;
  }
  long findSymbol(JIT* jit,const char *s){
    return (long)jit->findSymbol(s).getAddress().get();
  }

  long getSectionSize(JIT* jit,long addr){
    return (jit->map)[addr];
  }
  long getSectionStart(JIT* jit, long addr){
    for(auto x: (jit->map)){
      if(addr>=x.first&&addr<(x.first+x.second))
	return x.first;
    }
    return 0;
  }

  void dumpBinaryTo(const char *filename, long start, long size) {

  std::ofstream fs;
  fs.open(filename);
  for (int i = 0; i < size; i++) {
    fs << ((uint8_t *)start)[i];
  }
  }
  void printASMFrom(const char *filename) {
  char buffer[128];
  std::string s1 = "objdump -b binary -Mintel,x86-64 -m i386   -D ";
  std::string s2 = filename;
  std::string cmd = s1 + s2;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  while (fgets(buffer, 128, pipe.get()))
    printf("%s", buffer);
}
void printASM(long start, long size) {
  const char *c = "__temp_bin_llvm_debug__";
  dumpBinaryTo(c, start, size);
  printASMFrom(c);
}


}
