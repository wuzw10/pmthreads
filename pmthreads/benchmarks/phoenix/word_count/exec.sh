rm -f .crashed.log 
rm -f /tmp/nvlib.crash 
rm -rf /dev/shm/*
mkdir -p /dev/shm/nvthreads
mkdir -p /dev/shm/pmthreads
sleep 0.5
#config: pthread pmthreads pmthreads-mpt atlas nvthread
config=$1
if [ $# -eq 0 ] 	
then
	config=pthread
fi 	

export NVM_LATENCY="100"
export PERSIST_INTERVAL="100"
export CPU_FREQ_MHZ="2500"

TEST_NAME=${PWD##*/} 
CMD_ARGS=$(cat Makefile | grep "TEST_ARGS" | cut -d" "  -f 3- )
#echo "$CMD_ARGS"
PROG=./bin/$TEST_NAME-$config.out

if [ ! -f $PROG ]; then
    echo "$PROG not found!"
    ./build.sh $config
    sleep 0.5
fi

#time $PROG $CMD_ARGS
export TIMEFORMAT='TIME IS: %3R seconds'
#EXECUTE 
(time $PROG $CMD_ARGS) > /tmp/output  2>&1 
status=$(cat /tmp/output | grep "NORMAL EXIT")
if [ ! -z "$status" ]; then
	cat /tmp/output  | grep "TIME IS" | cut -d " " -f3-
	#cat /tmp/output | grep "real"
else
	echo "OPPS..."
	cat /tmp/output
fi


rm -f .crashed.log 
rm -f /tmp/nvlib.crash 
rm -rf /dev/shm/*
mkdir -p /dev/shm/nvthreads
mkdir -p /dev/shm/pmthreads
rm -f *.txt
