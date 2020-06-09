STATS_FLAGS=-DSTAT_BYTES
MALLOC_WRAPPER=Heap-Layers/wrappers/gnuwrapper.cpp
output=libpmthreads-verbose.so
CPPFLAGS=$(STATS_FLAGS) -std=c++14 -O3 -DNZW_DEBUG -DNDEBUG -ffast-math -fno-builtin-malloc -Wall -Wextra -Wshadow -Wconversion -Wuninitialized -mavx -msse2 -UNO_CRASH_TEST -UPARALLEL_COMMIT -DUSE_CLFLUSH_TO_COMMIT -UNO_TIMED_WAIT -Wno-format-overflow
