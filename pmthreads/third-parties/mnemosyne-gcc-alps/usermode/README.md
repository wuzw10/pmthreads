```
ALPS Dependencies (on Ubuntu) :
	cmake
    	libattr1-dev
    	libboost-all-dev
    	libevent-dev
    	libnuma1
    	libnuma-dev
    	libyaml-cpp-dev
	
Install the above dependencies and then type the commands below
at your terminal. Please find equivalent packages for your 
flavor of Linux. 

	$ cd usermode/library/pmalloc/include/alps
	$ mkdir build
	$ cd build
	$ cmake .. -DTARGET_ARCH_MEM=CC-NUMA -DCMAKE_BUILD_TYPE=Release
	$ make
```


```
build memcached 
scons --build-bench=memcached  

build stress-test 
scons --build-example=stress-test

#usage
#LIST 
./build/examples/stress-test/stress-test -n 80000 -p 40000 -t 4 -r 0.5 -e
#B+Tree
./build/examples/stress-test/stress-test -n 2000000 -p 1000000 -t 4 -r 0.5 
```
