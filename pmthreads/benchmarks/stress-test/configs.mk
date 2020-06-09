prog=pthread.out
LDFLAGS= -lpthread
CFLAGS=  -I. -I./util -I./lock-couple-list -g -std=c++11  -DPAGESIZE=4096 -UHOARD_NVMALLOC 
INS :=
