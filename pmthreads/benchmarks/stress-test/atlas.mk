prog=atlas.out
LDFLAGS= -L$(HOME)/pmthreads/third-parties/atlas/runtime/build2/lib -latlas -lpthread
INS=-Xclang -load -Xclang $(HOME)/pmthreads/third-parties/atlas/compiler-plugin/plugin_build/NvmInstrumenter.so
ATLAS_INCLUDE=$(HOME)/pmthreads/third-parties/atlas/runtime/build2/include
CFLAGS=  -I. -I$(ATLAS_INCLUDE)  -I./util -I./lock-couple-list -g -std=c++11  -DPAGESIZE=4096 -DATLAS_TEST
