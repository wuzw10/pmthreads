CONFIG=$1
if [ -z "$1" ]
then
	CONFIG=pthread
fi

NUM_THREADS=$2
if [ -z "$2" ]
then
	NUM_THREADS=4
fi

PUT_GET=$3
if [ -z "$3" ]
then
	PUT_GET=1:1
fi


DELTA_T=$4
if [ -z "$4" ]
then
       DELTA_T=100
fi 


#USAGE: config (pthread, pmthreads) num_threads  PUT:GET (1:1, e.g.) DELTA_T (10, 20, e.g.)

if [ "$CONFIG" == "pmthreads" ]
then
	export NVM_LATENCY="100"
	export CPU_FREQ_MHZ="2000"
	export PERSIST_INTERVAL="$DELTA_T"
	echo -n "Evaluate $CONFIG with $NUM_THREADS threads, PUT:GET=$PUT_GET DELTA_T $DELTA_T THROUGHPUT "
	#CLEAN NVM 
	rm -rf /dev/shm/* 
	mkdir -p /dev/shm/pmthreads
	rm -f .crashed.log
else
	echo -n "Evaluate $CONFIG with $NUM_THREADS threads, PUT:GET=$PUT_GET THROUGHPUT "
fi

memcached=$CONFIG.out

if [ ! -f ./$memcached ]
then 
	echo "$memcached not exist, now building" 
	cd $HOME/pmthreads/benchmarks/memcached/memcached-1.2.4
	./build.sh  > /tmp/memcached-build.log 2>&1
	cd $HOME/pmthreads/benchmarks/memcached/figure-7

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
	       	-c 50 --random-data  --show-config -n 2000\
	      	--data-size=32\
	       	--port=11215\
	      	--ratio=$PUT_GET\
		--threads=10\
		> /tmp/memtier_output 2>&1 
	#echo "kill $pid"
	kill -s SIGINT $pid  && sleep 1

	cat /tmp/memtier_output | grep "Totals"  | awk '{print $2}'
fi

if [ "$CONFIG" == "pmthreads" ]
then
	#CLEAN NVM 
	rm -rf /dev/shm/* 
	mkdir -p /dev/shm/pmthreads
	rm -f .crashed.log
fi
