TEST_NAME = dedup
TEST_FILES = binheap decoder dedup encoder hashtable queue rabin tree util

#ifeq ($(NCORES), 8)
#	THREADS = 2
#else
#	THREADS = 1
#endif

DATASET_HOME=$(HOME)/pmthreads/benchmarks/parsec/dataset/parsec-2.1/
#TEST_ARGS = -c -p -f -t 10 -i $(DATASET_HOME)/pkgs/kernels/dedup/inputs/FC-6-x86_64-disc1.iso -o output.dat.ddp
TEST_ARGS = -c -p -f -t 10 -i $(DATASET_HOME)/pkgs/kernels/dedup/inputs/media.dat -o output.dat.ddp

#CFLAGS = -D_XOPEN_SOURCE=600 -Iinclude -DENABLE_THREADS -DPARALLEL -std=gnu99 -D_XOPEN_SOURCE=600 -DMD32_REG_T=int -funroll-loops -fprefetch-loop-arrays -fstrict-aliasing -Wstrict-aliasing -O5 -fkeep-inline-functions -DOPENSSL_THREADS -D_REENTRANT -DL_ENDIAN -DTERMIO -DUSE_MMAP
CFLAGS = -g -Iinclude -DENABLE_THREADS -DPARALLEL  -DMD32_REG_T=int -D_REENTRANT -DL_ENDIAN  -D_XOPEN_SOURCE=600
#LIBS = -Llib -lssl -lz -lcrypto
LIBS = -lssl -lz -lcrypto
#LIBS =  /lib/i386-linux-gnu/libz.so.1.2.8 /lib/i386-linux-gnu/libcrypto.so.0.9.8  /lib/i386-linux-gnu/libssl.so.0.9.8

DMP_O_GRAN = 6
DMP_O_QUANTUM = 100000

DMP_B_GRAN = 6
DMP_B_QUANTUM = 100000

NVTHREADS_HOME=$(HOME)/pmthreads/third-parties/nvthreads



#CC = gcc -m32 -march=core2 -mtune=core2
#CXX = g++ -m32 -march=core2 -mtune=core2
CC = clang #-march=core2 -mtune=core2
CXX = clang++ #-march=core2 -mtune=core2
CFLAGS += -O3

#CONFIGS = pthread dthread dmp_o dmp_b
CONFIGS = nvthread pthread pmthreads pmthreads-p atlas
PROGS = $(addprefix $(TEST_NAME)-, $(CONFIGS))

.PHONY: default all clean

default: all
all: $(PROGS)
clean:
	rm -f $(PROGS) obj/* *.out

eval: $(addprefix eval-, $(CONFIGS))

############ pthread builders ############

PTHREAD_CFLAGS = $(CFLAGS)
PTHREAD_LIBS += $(LIBS) -lpthread

PTHREAD_OBJS = $(addprefix obj/, $(addsuffix -pthread.o, $(TEST_FILES)))

obj/%-pthread.o: %-pthread.c
	$(CC) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-pthread.o: %.c
	$(CC) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-pthread.o: %-pthread.cpp
	$(CXX) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-pthread.o: %.cpp
	$(CXX) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

$(TEST_NAME)-pthread: $(PTHREAD_OBJS)
	$(CC) $(PTHREAD_CFLAGS) -o $@.out $(PTHREAD_OBJS) $(PTHREAD_LIBS)

eval-pthread: $(TEST_NAME)-pthread
	./$(TEST_NAME)-pthread.out $(TEST_ARGS)
############ nvthread builders ############
NVTHREAD_CFLAGS = $(CFLAGS) -DNDEBUG
#DTHREAD_LIBS += $(LIBS) -rdynamic $(DTHREADS_HOME)/libdthreads64.so -ldl
NVTHREAD_LIBS += $(LIBS) -rdynamic $(NVTHREADS_HOME)/src/libnvthread.so -ldl

NVTHREAD_OBJS = $(addprefix obj/, $(addsuffix -nvthread.o, $(TEST_FILES)))

obj/%-nvthread.o: %-pthread.c
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-nvthread.o: %.c
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-nvthread.o: %-pthread.cpp
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/include

obj/%-nvthread.o: %.cpp
	$(CXX) $(NVTHREAD_CFLAGS) -c $< $(NVTHREADS_HOME)/src $(NVTHREADS_HOME)/src/source -o $@ -I$(HOME)/include -I$(NVTHREADS_HOME)/src/include

### FIXME, put the 
$(TEST_NAME)-nvthread: $(NVTHREAD_OBJS) $(NVTHREADS_HOME)/src/libnvthread.so
	$(CC) $(NVTHREAD_CFLAGS) -o $@.out $(NVTHREAD_OBJS) $(NVTHREAD_LIBS)

eval-nvthread: $(TEST_NAME)-nvthread
	./$(TEST_NAME)-nvthread.out $(TEST_ARGS)
#########pmthread builders #########################

include pmthreads.mk 
include pmthreads-mpt.mk 
include atlas.mk 
