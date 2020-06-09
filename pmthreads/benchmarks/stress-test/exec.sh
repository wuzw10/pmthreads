#exec.sh config num_threads read_ratio evaluation_type
CONFIG=$1
if [ -z "$1" ]
then
	CONFIG=pthread
fi

NUM_THREADS=$2
if [ -z "$2" ]
then
	NUM_THREADS=10
fi

READ_RATIO=$3
if [ -z "$3" ]
then
	READ_RATIO=0.5
fi 

#0:LINKED LIST 1:B+TREE 
TYPE=$4
if [ -z "$4" ]
then
	TYPE=1
fi 

#NVM EMULATION CONFIGS
export NVM_LATENCY="100"
export CPU_FREQ_MHZ="2000"
export PERSIST_INTERVAL="100"

#CLEAN NVM 
rm -rf /dev/shm/* 
mkdir -p /dev/shm/pmthreads
mkdir -p /dev/shm/nvthreads
rm -f .crashed.log /tmp/nvlib.crash



if [ $CONFIG == "mnemosyne"  ]
then
	PROG=$HOME/pmthreads/third-parties/mnemosyne-gcc-alps/usermode/build/examples/stress-test/stress-test
else
	PROG=./$CONFIG.out
fi


if [ $TYPE == "1" ]
then 
	TARGET='B+TREE'
	if [ $CONFIG == "atlas" ] || [ $CONFIG == "mnemosyne" ] || [ $CONFIG == "nvthreads" ] 
	then 
		ARGUMENTS=" -n 200000 -p 100000 " #ATLAS crashed during warming up with conventional input.
	else
		ARGUMENTS=" -n 2000000 -p 1000000 "
	fi
else
	TARGET='LINKED LIST'
	if [ $CONFIG == "atlas" ] || [ $CONFIG == "nvthreads" ] 
	then 
		ARGUMENTS=" -n 8000 -p 4000 " #ATLAS crashed during warming up with conventional input.
	else
		ARGUMENTS=" -n 80000 -p 40000 "
	fi
fi 

echo -n "$TARGET STRESS TEST EVALUATION: $CONFIG ($NUM_THREADS threads, $READ_RATIO read-only ops) "

if [ $CONFIG == "mnemosyne" ]
then
	cp ./inputs/uniform_input.txt /tmp/
	cd $HOME/pmthreads/third-parties/mnemosyne-gcc-alps/usermode/
	if [ $TYPE == "1" ]
	then 
		$PROG $ARGUMENTS -t $NUM_THREADS -r $READ_RATIO > /tmp/stress_test_output 2>&1 
	else
		$PROG $ARGUMENTS -t $NUM_THREADS -r $READ_RATIO -e > /tmp/stress_test_output 2>&1
	fi
else
	$PROG $ARGUMENTS -t $NUM_THREADS -r $READ_RATIO -e $TYPE > /tmp/stress_test_output 2>&1
fi


THROUGHPUT=$(cat /tmp/stress_test_output | grep "Aggregated Throughput" | cut -d" " -f3 )

if [ ! -z $THROUGHPUT ]
then 
	echo "THROUGHPUT $THROUGHPUT (ops/ms) "
else
	cat /tmp/stress_test_output
fi


#CLEAN NVM 
rm -rf /dev/shm/* 
mkdir -p /dev/shm/pmthreads
mkdir -p /dev/shm/nvthreads
rm -f .crashed.log /tmp/nvlib.crash
