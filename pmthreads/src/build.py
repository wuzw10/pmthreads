import os
import sys

with_malloc_wrapper = True
suffix=".so"

cppflags= "CPPFLAGS=$(STATS_FLAGS) -std=c++14 -O3 -DNZW_DEBUG -DNDEBUG -ffast-math -fno-builtin-malloc -Wall -Wextra -Wshadow -Wconversion -Wuninitialized -mavx -msse2 -UNO_CRASH_TEST -UPARALLEL_COMMIT -DUSE_CLFLUSH_TO_COMMIT -UNO_TIMED_WAIT -Wno-format-overflow" 

prefix='libpmthreads'
with_malloc_wrapper = False
zero_test = False
build_mpt = False
if len(sys.argv) < 2:
    print 'Usage: python build.py config (pmthreads malloc zero mpt mpt-malloc)'
    exit()
elif sys.argv[1] == 'pmthreads-verbose':
    with_malloc_wrapper = True
    prefix+='-verbose'
elif sys.argv[1] == 'pmthreads-malloc':
    with_malloc_wrapper = True
    prefix+='-malloc'
    #cppflags+=' -DPARSEC_TEST'
elif sys.argv[1] == 'pmthreads-zero':
    zero_test = True
    cppflags+=" -DZERO_TEST"
    prefix+="-zero"
elif sys.argv[1] == 'pmthreads-mpt':
    build_mpt = True
    cppflags += " -DTRACK_PAGE_PROTECTION_FAULT"
    prefix+="-mpt"
elif sys.argv[1] == 'pmthreads-mpt-malloc':
    build_mpt = True
    with_malloc_wrapper = True
    cppflags += " -DTRACK_PAGE_PROTECTION_FAULT"
    prefix+="-mpt-malloc"
    cppflags+=' -DPARSEC_TEST'
else:
    print 'Normal build.'

output=prefix
f = open("./configs.mk", "w")

if sys.argv[1] == 'pmthreads-verbose':
    f.write("STATS_FLAGS=-DSTAT_BYTES\n")
else:
    f.write("STATS_FLAGS=-USTAT_BYTES\n")
if with_malloc_wrapper == True:
    f.write("MALLOC_WRAPPER=Heap-Layers/wrappers/gnuwrapper.cpp\n")
if build_mpt == True:
    f.write("EXTRA_SRC=source/signal_handler.cpp\n")
output+=suffix 
f.write("output=" + output + "\n")
f.write(cppflags + "\n")
f.close()

print 'building ' + output 
os.system("make")
target = output
