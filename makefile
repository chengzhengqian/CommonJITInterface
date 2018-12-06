LLVM_CONFIG= llvm-config-5.0
LLVM_INCLUDE_DIR := $(shell  $(LLVM_CONFIG) --includedir)
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --libs --system-libs --cflags --ldflags all)
# GUILE_CONFIG=guile-config
GUILE_CONFIG=/home/chengzhengqian/cloud/guile2.2/bin/guile-config
GUILE_CFLAGS=$(shell $(GUILE_CONFIG)  compile)
GUILE_LIBS=$(shell $(GUILE_CONFIG)  link)
LLVMCXX=$(shell llvm-config-5.0 --cxxflags  --ldflags --libs --system-libs  all)

# all: llvm-guile _llvm.so
all: llvm-guile


llvm_wrap_py.c: llvm.swig
	swig  -o $@ -python -I$(LLVM_INCLUDE_DIR)  $^
_llvm.so: llvm_wrap_py.c
	gcc -shared -fPIC $^ -I/usr/include/python2.7/ -o $@ -I$(LLVM_INCLUDE_DIR)  $(LLVM_LDFLAGS)

array.h: array.c
	./makeheaders $<
array.o: array.c array.h
	gcc -c -O3 -o $@  -I$(LLVM_INCLUDE_DIR) $<

mcjit.h: mcjit.c
	./makeheaders $<
mcjit.o: mcjit.c mcjit.h
	gcc -c -O3 -o $@  -I$(LLVM_INCLUDE_DIR) $<

container.o: container.cpp container.h
	g++ -std=c++14 -c -O3 -o $@  $<

runFunc.h: runFunc.c
	./makeheaders $<
runFunc.o: runFunc.c runFunc.h
	gcc -c -O3 -o $@  -I$(LLVM_INCLUDE_DIR) $<

JIT.o: JIT.cpp JIT.h JIT_imp.cpp
	g++ -c -o $@  $< $(LLVMCXX)

test_JIT.o: test_JIT.c
	gcc -c -O3 -o $@  -I$(LLVM_INCLUDE_DIR) $<
test_JIT: test_JIT.o JIT.o
	g++ -o $@ $^  $(LLVM_LDFLAGS)
missing.h: missing.c 
	./makeheaders $<
missing.o: missing.c missing.h
	gcc -c -O3 -o $@ -I$(LLVM_INCLUDE_DIR)  $< 

llvm_wrap.c: llvm.swig array.h missing.h JIT.h runFunc.h mcjit.h  container.h
	swig -guile -I$(LLVM_INCLUDE_DIR) $<
llvm_wrap.o: llvm_wrap.c  missing.h
	gcc -c -O3 -o $@  $< -I$(LLVM_INCLUDE_DIR)  $(GUILE_CFLAGS)  


main.o:main_guile.c llvm_wrap.h 
	gcc -c -o $@ $<  -I$(LLVM_INCLUDE_DIR) $(GUILE_CFLAGS) 

llvm-guile:  main.o array.o missing.o llvm_wrap.o JIT.o runFunc.o mcjit.o  container.o
	g++ -o $@ $^ $(GUILE_LIBS)  $(LLVM_LDFLAGS)

