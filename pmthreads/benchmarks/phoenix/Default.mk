NVTHREADS_HOME=$(HOME)/pmthreads/third-parties/nvthreads/src
PMTHREADS_HOME=$(HOME)/pmthreads/src
ATLAS_HOME=$(HOME)/pmthreads/third-parties/atlas/runtime/build/lib

NTHREADS ?=8

CC = clang
CXX = clang++
CFLAGS += -O3 -g

CONFIGS = pthread nvthread pmthreads atlas pmthreads-mpt
PROGS = $(addprefix $(TEST_NAME)-, $(CONFIGS))

.PHONY: default all clean

default: all
all: $(PROGS)
clean:
	@rm -f $(PROGS) obj/* *.out

eval: $(addprefix eval-, $(CONFIGS))

############ pthread builders ############

PTHREAD_CFLAGS = $(CFLAGS)  -DNDEBUG
PTHREAD_LIBS += $(LIBS) -lpthread

PTHREAD_OBJS = $(addprefix obj/, $(addsuffix -pthread.o, $(TEST_FILES)))

obj/%-pthread.o: %-pthread.c
	$(CC) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pthread.o: %.c
	$(CC) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pthread.o: %-pthread.cpp
	$(CXX) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pthread.o: %.cpp
	$(CXX) $(PTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

$(TEST_NAME)-pthread: $(PTHREAD_OBJS)
	$(CC) $(PTHREAD_CFLAGS) -o $@.out $(PTHREAD_OBJS) $(PTHREAD_LIBS)

eval-pthread: $(TEST_NAME)-pthread
	perf stat -d -r 10      ./$(TEST_NAME)-pthread.out $(TEST_ARGS)


############ nvthread builders ############
NVTHREAD_CFLAGS = $(CFLAGS) -DNDEBUG
#DTHREAD_LIBS += $(LIBS) -rdynamic $(DTHREADS_HOME)/libdthreads64.so -ldl
ifneq (, $(findstring -m32, $(CFLAGS)))
	NVTHREAD_LIBS += $(LIBS) -rdynamic $(NVTHREADS_HOME)/libnvthread32.so -ldl
else
	NVTHREAD_LIBS += $(LIBS) -rdynamic $(NVTHREADS_HOME)/libnvthread.so -ldl
endif

NVTHREAD_OBJS = $(addprefix obj/, $(addsuffix -nvthread.o, $(TEST_FILES)))

obj/%-nvthread.o: %-pthread.c
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-nvthread.o: %.c
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-nvthread.o: %-pthread.cpp
	$(CC) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-nvthread.o: %.cpp
	$(CXX) $(NVTHREAD_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. 

### FIXME, put the 
$(TEST_NAME)-nvthread: $(NVTHREAD_OBJS) $(NVTHREADS_HOME)/libnvthread.so
	$(CC) $(NVTHREAD_CFLAGS) -o $@.out $(NVTHREAD_OBJS) $(NVTHREAD_LIBS)

eval-nvthread: $(TEST_NAME)-nvthread
	perf stat -d -r 10      ./$(TEST_NAME)-nvthread.out $(TEST_ARGS)


############ pmthreads builders ############
PMTHREADS_CFLAGS = $(CFLAGS) -DNDEBUG -Xclang -load -Xclang $(PMTHREADS_HOME)/compiler-plugin/plugin_build/libPMThreadsPass.so
PMTHREADS_LIBS += $(LIBS) -rdynamic $(PMTHREADS_HOME)/libpmthreads-malloc.so -ldl -lpthread 
CLANG=clang
CLANGPP=clang++
PMTHREADS_OBJS = $(addprefix obj/, $(addsuffix -pmthreads.o, $(TEST_FILES)))

obj/%-pmthreads.o: %-pthread.c
	$(CLANG) $(PMTHREADS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads.o: %.c
	$(CLANG) $(PMTHREADS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads.o: %-pthread.cpp
	$(CLANG) $(PMTHREADS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads.o: %.cpp
	$(CLANGPP) $(PMTHREADS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. 

### FIXME, put the 
$(TEST_NAME)-pmthreads: $(PMTHREADS_OBJS) $(PMTHREADS_HOME)/libpmthreads-malloc.so
	$(CLANGPP) $(PMTHREADS_CFLAGS) -o $@.out $(PMTHREADS_OBJS) $(PMTHREADS_LIBS) 

eval-pmthreads: $(TEST_NAME)-pmthreads
	perf stat -d -r 10      ./$(TEST_NAME)-pmthreads.out $(TEST_ARGS)
############ atals builders ############
ATLAS_CFLAGS = $(CFLAGS)  -DNDEBUG -DATLAS_TEST -Xclang -load -Xclang $(HOME)/pmthreads/third-parties/atlas/compiler-plugin/plugin_build/NvmInstrumenter.so
ATLAS_LIBS += $(LIBS) -lpthread
CLANG=clang
CLANGPP=clang++
ATLAS_OBJS = $(addprefix obj/, $(addsuffix -atlas.o, $(TEST_FILES)))

obj/%-atlas.o: %-pthread.c
	$(CLANG) $(ATLAS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. -I$(HOME)/pmthreads/third-parties/atlas/runtime/build/include

obj/%-atlas.o: %.c
	$(CLANG) $(ATLAS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. -I$(HOME)/pmthreads/third-parties/atlas/runtime/build/include

obj/%-atlas.o: %-pthread.cpp
	$(CLANG) $(ATLAS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. -I$(HOME)/pmthreads/third-parties/atlas/runtime/build/include

obj/%-atlas.o: %.cpp
	$(CLANGPP) $(ATLAS_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. -I$(HOME)/pmthreads/third-parties/atlas/runtime/build/include

### FIXME, put the 
$(TEST_NAME)-atlas: $(ATLAS_OBJS) 
	$(CLANGPP) $(ATLAS_CFLAGS) $(ATLAS_OBJS) -L$(ATLAS_HOME) -latlas  -o $@.out $(ATLAS_LIBS) 

eval-atlas: $(TEST_NAME)-atlas
	perf stat -d   ./$(TEST_NAME)-atlas.out $(TEST_ARGS)
############ pmthreads-mpt builders ############
PMTHREADS-MPT_CFLAGS := $(CFLAGS) -DNDEBUG 
PMTHREADS-MPT_LIBS := $(LIBS) -rdynamic $(PMTHREADS_HOME)/libpmthreads-mpt-malloc.so -ldl -lpthread 
CLANG=clang
CLANGPP=clang++
PMTHREADS-MPT_OBJS = $(addprefix obj/, $(addsuffix -pmthreads-mpt.o, $(TEST_FILES)))

obj/%-pmthreads-mpt.o: %-pthread.c
	$(CLANG) $(PMTHREADS-MPT_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads-mpt.o: %.c
	$(CLANG) $(PMTHREADS-MPT_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads-mpt.o: %-pthread.cpp
	$(CLANG) $(PMTHREADS-MPT_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I.

obj/%-pmthreads-mpt.o: %.cpp
	$(CLANGPP) $(PMTHREADS-MPT_CFLAGS) -c $< -o $@ -I$(HOME)/pmthreads/benchmarks/phoenix/include -I. 

### FIXME, put the 
$(TEST_NAME)-pmthreads-mpt: $(PMTHREADS-MPT_OBJS) $(PMTHREADS_HOME)/libpmthreads-mpt-malloc.so
	$(CLANGPP) $(PMTHREADS-MPT_CFLAGS) -o $@.out $(PMTHREADS-MPT_OBJS) $(PMTHREADS-MPT_LIBS) 

eval-pmthreads-mpt: $(TEST_NAME)-pmthreads-mpt
	perf stat -d     ./$(TEST_NAME)-pmthreads-mpt.out $(TEST_ARGS)
