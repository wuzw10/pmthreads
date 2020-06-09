CONFIGS = pthread nvthread atlas pmthreads pmthreads-mpt
PROGS = $(addprefix $(PROG)-, $(CONFIGS))
#NUM_THREADS=20

.PHONY: default all clean

default: all
all: $(PROGS)
eval: $(addprefix eval-, $(CONFIGS))

CC       := clang 
CFLAGS   += -g -Wall 
CFLAGS   += -O3
INCLUDE = -I.
CFLAGS   += $(INCLUDE)
CPP      := clang++ 
CPPFLAGS += $(CFLAGS)
LD       := clang++ 
LIBS     = 
OUTPUT +=

########### clean commands ##################
clean:
	rm -f *.out $(OBJS)
