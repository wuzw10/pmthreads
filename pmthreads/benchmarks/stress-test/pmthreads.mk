prog=pmthreads.out
LDFLAGS= -rdynamic $(HOME)/pmthreads/src/libpmthreads.so -ldl -lpthread
CFLAGS=  -I. -I./util -I./lock-couple-list -g -std=c++11  -DPAGESIZE=4096 -DPMTHREADS_TEST -DHOARD_NVMALLOC
INS := -Xclang -load -Xclang $(HOME)/pmthreads/src/compiler-plugin/plugin_build/libPMThreadsPass.so 
