prog=nvthreads.out
LDFLAGS=-rdynamic $(HOME)/pmthreads/third-parties/nvthreads/src/libnvthread.so -ldl -lpthread
CFLAGS=  -I. -I./util -I./lock-couple-list -g -std=c++11  -DPAGESIZE=4096 -DNVTHREADS_TEST
INS :=
