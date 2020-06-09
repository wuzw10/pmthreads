prog=atlas.out
LDFLAGS=  -L$(HOME)/pmthreads/third-parties/atlas/runtime/build2/lib -latlas -lpthread
INS=-Xclang -load -Xclang $(HOME)/pmthreads/third-parties/atlas/compiler-plugin/plugin_build/NvmInstrumenter.so
CFLAGS=  -I. -I$(HOME)/pmthreads/third-parties/atlas/runtime/build2/include -std=c++11 -DATLAS_TEST

.PHONY:	all clean run

cppsrcs= \
	barrier.cpp  get_options.cpp  main.cpp concurrent_btree.cpp 

csrcs = \
	coupling.c linkedlist-lock.c		

cppobjs := $(patsubst %.cpp,%.o,$(cppsrcs))
cobjs := $(patsubst %.c,%.o,$(csrcs))
objs := $(cppobjs) 
objs += $(cobjs)

CC=clang++

CFLAGS+= -O3

all: $(prog)

run: $(prog)
	./$(prog)


%.o:%.c
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@ 

$(prog): $(objs) 
	$(CC) $^ -o $@  $(LDFLAGS)

clean:
	rm -f $(objs) $(prog) *.obj .crashed.log
