.PHONY: all clean



PMTHREADS= -rdynamic $(HOME)/pmthreads/src/libpmthreads-verbose.so -ldl -lpthread

prog=pmthreads-verbose.out

all: $(prog)

CC=clang 
INS= -Xclang -load -Xclang $(HOME)/pmthreads/src/compiler-plugin/plugin_build/libPMThreadsPass.so -DUSE_SYSTEM_MALLOC









CFLAGS= -DUSE_THREADS 
$(prog):
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I.  -DNDEBUG   -g -O2 -MT memcached-memcached.o -MD -MP -MF .deps/memcached-memcached.Tpo -c -o memcached-memcached.o `test -f 'memcached.c' || echo './'`memcached.c
	mv -f .deps/memcached-memcached.Tpo .deps/memcached-memcached.Po
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I.  -DNDEBUG  -DUSE_SYSTEM_MALLOC   -g -O2 -MT memcached-slabs.o -MD -MP -MF .deps/memcached-slabs.Tpo -c -o memcached-slabs.o `test -f 'slabs.c' || echo './'`slabs.c
	mv -f .deps/memcached-slabs.Tpo .deps/memcached-slabs.Po
	$(CC) $(CFLAGS)  $(INS) -DHAVE_CONFIG_H -I.  -DNDEBUG   -g -O2 -MT memcached-items.o -MD -MP -MF .deps/memcached-items.Tpo -c -o memcached-items.o `test -f 'items.c' || echo './'`items.c
	mv -f .deps/memcached-items.Tpo .deps/memcached-items.Po
	$(CC) $(CFLAGS)  $(INS) -DHAVE_CONFIG_H -I. -DNDEBUG   -g -O2 -MT memcached-assoc.o -MD -MP -MF .deps/memcached-assoc.Tpo -c -o memcached-assoc.o `test -f 'assoc.c' || echo './'`assoc.c
	mv -f .deps/memcached-assoc.Tpo .deps/memcached-assoc.Po
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I.  -DNDEBUG   -g -O2 -MT memcached-thread.o -MD -MP -MF .deps/memcached-thread.Tpo -c -o memcached-thread.o `test -f 'thread.c' || echo './'`thread.c
	mv -f .deps/memcached-thread.Tpo .deps/memcached-thread.Po
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I.  -DNDEBUG   -g -O2 -MT memcached-stats.o -MD -MP -MF .deps/memcached-stats.Tpo -c -o memcached-stats.o `test -f 'stats.c' || echo './'`stats.c
	mv -f .deps/memcached-stats.Tpo .deps/memcached-stats.Po
	$(CC) $(CFLAGS)  -g -O2   -o $(prog) memcached-memcached.o memcached-slabs.o memcached-items.o memcached-assoc.o memcached-thread.o memcached-stats.o  $(PMTHREADS)  -levent 

clean:
	rm -f *.o $(prog)
