CONFIG=pmthreads-verbose
PUT_GET=1:1

DELTA_T=$1
if [ -z "$1" ]
then
	DELTA_T=100
fi 

NUM_THREADS=$2
if [ -z "$2" ]
then
	NUM_THREADS=10
fi

#USAGE: DELTA_T num_threads

export NVM_LATENCY="100"
export CPU_FREQ_MHZ="2000"
export PERSIST_INTERVAL="$DELTA_T"
echo "Evaluate $CONFIG with $NUM_THREADS threads, PUT:GET=$PUT_GET DELTA_T $DELTA_T"
#CLEAN NVM 
rm -rf /dev/shm/* 
mkdir -p /dev/shm/pmthreads
rm -f .crashed.log

memcached=$CONFIG.out

if [ ! -f ./$memcached ]
then 
	echo "$memcached not exist, now building" 
	cd $HOME/pmthreads/benchmarks/memcached/memcached-1.2.4
	make -f pmthreads-verbose.mk clean  > /tmp/memcached-build.log 2>&1
	make -f pmthreads-verbose.mk  > /tmp/memcached-build.log 2>&1
	cd $HOME/pmthreads/benchmarks/memcached/figure-8
	cp $HOME/pmthreads/benchmarks/memcached/memcached-1.2.4/pmthreads-verbose.out . 

	if [ ! -f ./$memcached ]
	then
		echo "failed to build $memcached."
		cat /tmp/memcached-build.log
		exit 
	fi
else
	user=$(whoami)
	numactl --cpunodebind=0 ./$memcached -l 127.0.0.1 -p 11215 -u $user -t $NUM_THREADS -p 11215 >  /tmp/memcached_output  2>&1  & 
	pid=$!
	#echo "memcached pid is $pid."
	numactl --cpunodebind=0 memtier_benchmark --server=127.0.0.1 --hide-histogram --protocol=memcache_text \
		-c 20 --random-data  --show-config\
		--data-size=32\
		--port=11215\
		--ratio=$PUT_GET\
		--threads=10\
		--test-time=10\
		> /tmp/memtier_output 2>&1 
	#echo "kill $pid"
	kill -s SIGINT $pid  && sleep 1
	retval="$?"
	if [ "$retval" -ne 0 ]
	then
		echo "somthing wrong ..., please retry."
		exit $retval
	fi
fi

cat /tmp/memcached_output  | grep "COMMIT STATISTICS"


#CLEAN NVM 
rm -rf /dev/shm/* 
mkdir -p /dev/shm/pmthreads
rm -f .crashed.log
