PMTHREADS_P_HOME=$(HOME)/pmthreads
PMTHREADS_P_CFLAGS = $(CFLAGS) -DNDEBUG 
PMTHREADS_P_LIBS += $(LIBS) -rdynamic $(PMTHREADS_P_HOME)/src/libpmthreads-mpt-malloc.so -ldl -lpthread
PMTHREADS_P_OBJS = $(addprefix obj/, $(addsuffix -pmthreads-p.o, $(TEST_FILES)))

obj/%-pmthreads-p.o: %-pthread.c
	$(CC) $(PMTHREADS_P_CFLAGS) -c $< -o $@ 

obj/%-pmthreads-p.o: %.c
	$(CC) $(PMTHREADS_P_CFLAGS) -c $< -o $@ 

obj/%-pmthreads-p.o: %-pthread.cpp
	$(CC) $(PMTHREADS_P_CFLAGS) -c $< -o $@ 

obj/%-pmthreads-p.o: %.cpp
	$(CXX) $(PMTHREADS_P_CFLAGS) -c $< $(PMTHREADS_P_HOME)/src $(PMTHREADS_P_HOME)/src/source -o $@ 

### FIXME, put the 
$(TEST_NAME)-pmthreads-mpt: $(PMTHREADS_P_OBJS) $(PMTHREADS_P_HOME)/src/libpmthreads-mpt-malloc.so
	$(CC) $(PMTHREADS_P_CFLAGS) -o $@.out $(PMTHREADS_P_OBJS) $(PMTHREADS_P_LIBS)
