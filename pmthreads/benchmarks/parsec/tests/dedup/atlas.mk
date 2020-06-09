ATLAS_CFLAGS = $(CFLAGS) -DNDEBUG -lstdc++#-lstdc# -std=c++11
ATLAS_CFLAGS += -DATLAS_TEST $(CFLAGS) -Xclang -load -Xclang  $(HOME)/pmthreads/third-parties/atlas/compiler-plugin/plugin_build/NvmInstrumenter.so -I$(HOME)/pmthreads/third-parties/atlas/runtime/build/include
ATLAS_LIBS += $(LIBS)  -L$(HOME)/pmthreads/third-parties/atlas/runtime/build/lib -latlas -lpthread
ATLAS_OBJS = $(addprefix obj/, $(addsuffix -atlas.o, $(TEST_FILES)))

obj/%-atlas.o: %-pthread.c
	$(CC) $(ATLAS_CFLAGS) -c $< -o $@ 

obj/%-atlas.o: %.c
	$(CC) $(ATLAS_CFLAGS) -c $< -o $@ 

obj/%-atlas.o: %-pthread.cpp
	$(CC) $(ATLAS_CFLAGS) -c $< -o $@ 

obj/%-atlas.o: %.cpp
	$(CXX) $(ATLAS_CFLAGS) -c $< -o $@ 

$(TEST_NAME)-atlas: $(ATLAS_OBJS) $(HOME)/pmthreads/third-parties/atlas/runtime/build/lib/libatlas.a
	$(CC) $(ATLAS_CFLAGS) -o $@.out $(ATLAS_OBJS) $(ATLAS_LIBS)
