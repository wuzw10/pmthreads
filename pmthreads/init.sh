./install-llvm.sh

PWD=$(pwd)
MAIN=$PWD
if [ ! $PWD == $HOME/pmthreads  ]
then
       echo "Please move this project to $HOME/."	
fi 

echo "BUILDING pmthreads ... "
cd $HOME/pmthreads/src 
./build.sh
cd $HOME/pmthreads/src/compiler-plugin
./build_plugin > /tmp/build_log 2>&1
if [ ! -f $HOME/pmthreads/src/compiler-plugin/plugin_build/libPMThreadsPass.so ]
then 
	cat /tmp/build_log
	exit
fi
cd $MAIN


echo "BUILDING atlas ..."
cd $HOME/pmthreads/third-parties/atlas/compiler-plugin
./build_plugin > /tmp/build_log 2>&1
if [ ! -f $HOME/pmthreads/third-parties/atlas/compiler-plugin/plugin_build/NvmInstrumenter.so ]
then 
	cat /tmp/build_log
	exit
fi
cd $MAIN

cd $HOME/pmthreads/third-parties/atlas/runtime 
./build.sh  > /tmp/build_log 2>&1 
if [ ! -f $HOME/pmthreads/third-parties/atlas/runtime/build/lib/libatlas.a ]
then
	cat /tmp/build_log
	exit 	
fi 


cd $MAIN

echo "BUILDING nvthread ... "
cd $HOME/pmthreads/third-parties/nvthreads/src
make clean > /tmp/build_log 2>&1 
make > /tmp/build_log 2>&1
if [ ! -f $HOME/pmthreads/third-parties/nvthreads/src/libnvthread.so ] || [ ! -f $HOME/pmthreads/third-parties/nvthreads/src/libnvthread32.so ] 
then 
	cat /tmp/build_log
	exit 
fi 


echo "BUILDING alps ..."
cd $HOME/pmthreads/third-parties/mnemosyne-gcc-alps/
cd usermode/library/pmalloc/include/alps
rm -rf  build
mkdir build
cd build
cmake .. -DTARGET_ARCH_MEM=CC-NUMA -DCMAKE_BUILD_TYPE=Release > /tmp/build_log 2>&1 
make > /tmp/build_log 2>&1 

status=$( cat /tmp/build_log | grep "Built target alps" )

if [ -z "$status" ]
then
	cat /tmp/build_log
	exit 	
fi 

echo "BUILDING parsec ..."
cd $MAIN
#takes 11m54.523s
#build parsec 
MAIN=$(pwd)
cd benchmarks/parsec/
./download-input.sh
cd $MAIN
cd benchmarks/parsec/
cd ./tests/
./build.sh

echo "BUILDING phoenix ... "
#build phoenix 
cd $MAIN
cd benchmarks/phoenix/
./download-input.sh 
cd $MAIN
cd benchmarks/phoenix/
./build.sh

echo "BUILDING stress tests ... "
#build stress tests
cd $MAIN
cd benchmarks/stress-test
./build.sh  

echo "BUILDING memcached ... "
#clone & install memtier benchmark 
cd $MAIN
cd benchmarks/memcached/memtier_benchmark 
autoreconf -ivf > /tmp/memtier_install.log 2>&1
./configure >>  /tmp/memtier_install.log 2>&1
make	>>  /tmp/memtier_install.log 2>&1
make install >>  /tmp/memtier_install.log 2>&1
if [ ! -f "/usr/local/bin/memtier_benchmark" ]
then
	cat /tmp/memtier_install.log
fi 



#build memcached 
cd $MAIN
cd benchmarks/memcached
cd ./memcached-1.2.4 
./build.sh 

echo "BUILDING recovery test ... "
#build recovery-test
cd $MAIN
cd benchmarks/recovery-test
./build.sh 

echo "BUILDING DONE."
pip install scipy==0.16
