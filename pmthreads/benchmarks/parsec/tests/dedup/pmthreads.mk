PMTHREADS_HOME=$(HOME)/pmthreads

PMTHREADS_CFLAGS = $(CFLAGS) -DNDEBUG 
PMTHREADS_CFLAGS +=  -Xclang -load -Xclang $(PMTHREADS_HOME)/src/compiler-plugin/plugin_build/libPMThreadsPass.so
PMTHREADS_LIBS += $(LIBS) -rdynamic $(PMTHREADS_HOME)/src/libpmthreads-malloc.so -ldl -lpthread
PMTHREADS_OBJS = $(addprefix obj/, $(addsuffix -pmthreads.o, $(TEST_FILES)))

obj/%-pmthreads.o: %-pthread.c
	$(CC) $(PMTHREADS_CFLAGS) -c $< -o $@ 

obj/%-pmthreads.o: %.c
	$(CC) $(PMTHREADS_CFLAGS) -c $< -o $@ 

obj/%-pmthreads.o: %-pthread.cpp
	$(CC) $(PMTHREADS_CFLAGS) -c $< -o $@ 

obj/%-pmthreads.o: %.cpp
	$(CXX) $(PMTHREADS_CFLAGS) -c $< $(PMTHREADS_HOME)/src $(PMTHREADS_HOME)/src/source -o $@ 

### FIXME, put the 
$(TEST_NAME)-pmthreads: $(PMTHREADS_OBJS) $(PMTHREADS_HOME)/src/libpmthreads-malloc.so
	$(CC) $(PMTHREADS_CFLAGS) -o $@.out $(PMTHREADS_OBJS) $(PMTHREADS_LIBS)
