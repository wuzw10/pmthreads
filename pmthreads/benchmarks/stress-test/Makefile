.PHONY:	all clean run

cppsrcs= \
		 main.cpp barrier.cpp  get_options.cpp 

csrcs = \
	linkedlist-lock.c  coupling.c 

cppobjs := $(patsubst %.cpp,%.o,$(cppsrcs))
cobjs := $(patsubst %.c,%.o,$(csrcs))
objs := $(cppobjs) 
objs += $(cobjs)

CC=clang++

include configs.mk

CFLAGS+= -O3

all: $(prog)

run: $(prog)
	rm -f .crashed.log
	./$(prog)


%.o:%.c
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@

concurrent_btree.obj:concurrent_btree.cxx
	$(CC) $(CFLAGS) $(INS) -c $^ -o $@ 

%.o:%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@ 

$(prog): $(objs) concurrent_btree.obj 
	$(CC) $^ -o $@  $(LDFLAGS)

clean:
	rm -f $(objs) $(prog) *.obj .crashed.log
