prog=pmthreads.out
LDFLAGS= -rdynamic $(HOME)/pmthreads/src/libpmthreads.so -ldl -lpthread
CFLAGS=  -I. -I./util -I./lock-couple-list -g -std=c++11  -DPMTHREADS 
INS := -Xclang -load -Xclang $(HOME)/pmthreads/src/compiler-plugin/plugin_build/libPMThreadsPass.so 

.PHONY:	all clean run

cppsrcs= \
	barrier.cpp  get_options.cpp  main.cpp concurrent_btree.cpp 

csrcs = \
	coupling.c linkedlist-lock.c		
#lock-couple-list/linkedlist-lock.c  lock-couple-list/coupling.c 

cppobjs := $(patsubst %.cpp,%.o,$(cppsrcs))
cobjs := $(patsubst %.c,%.o,$(csrcs))
objs := $(cppobjs) 
objs += $(cobjs)

CC=clang++

#include configs.mk

CFLAGS+= -O3

all: $(prog)

run: $(prog)
	rm -f .crashed.log
	./$(prog)


%.o:%.c
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@ 

$(prog): $(objs) 
	$(CC) $^ -o $@  $(LDFLAGS)

clean:
	rm -f $(objs) $(prog) *.obj .crashed.log
