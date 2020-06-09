include ./configs.mk

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
all: Heap-Layers $(UNAME_S)-gcc-$(UNAME_P)

help:
	@echo Currently, we only support Linux-gcc-x86_64
	@echo Linux-gcc-x86_64
	

.PHONY: Linux-gcc-x86_64

#
# Source files
#

MAIN_SRC  = source/pmthreads.cpp
UNIX_SRC  = $(MAIN_SRC) source/real.cpp source/internal_heap.cpp source/dthreadheap.cpp source/logger.cpp $(EXTRA_SRC)
GNU_SRC   = $(UNIX_SRC) $(MALLOC_WRAPPER)#Heap-Layers/wrappers/gnuwrapper.cpp 

#
# All dependencies.
#
DEPS = Heap-Layers $(MACOS_SRC) $(UNIX_SRC) source/libhoard.cpp

Heap-Layers:
	git clone https://github.com/emeryberger/Heap-Layers

#
# Include directories
#

INCLUDES = -I. -Iinclude  -IHeap-Layers  -Iinclude/dthread-heaps

LINUX_GCC_x86_64_COMPILE = $(CXX) $(CPPFLAGS) -g -W -Wconversion -Wall -I/usr/include/nptl -fno-builtin-malloc -pipe -fPIC -DNDEBUG  $(INCLUDES) -D_REENTRANT=1 -shared   $(GNU_SRC) -Bsymbolic -o $(output) -ldl -lpthread


Linux-gcc-x86_64:
	$(LINUX_GCC_x86_64_COMPILE)

clean:
	rm -rf libhoard.* libpmthreads.*
