DELTA_T=$1

if [ -z "$1" ]
then
	DELTA_T=100
fi 


TYPE=$2

if [ -z "$2" ]
then
	TYPE=0
fi


#CLEAN NVM
rm -rf /dev/shm/*
mkdir -p /dev/shm/pmthreads
rm -f .crashed.log

export NVM_LATENCY="100"
export CPU_FREQ_MHZ="2000"
export PERSIST_INTERVAL="$DELTA_T"




if [ ! -f pmthreads.out ]
then
	echo "pmthreads.out not exist, now building ..."
	make -f pmthreads.mk clean > /dev/null 2>&1 
	make -f pmthreads.mk > /tmp/build_log 2>&1 

	if [ ! -f pmthreads.out ]
	then
		cat /tmp/build_log
	fi
fi 

if [ $TYPE == "0" ]
then 
	echo "LINKED LIST RECOVERY TEST WITH PMTHREADS (DELTA_T=$DELTA_T)"
else
	echo "B+TREE RECOVERY TEST WITH PMTHREADS (DELTA_T=$DELTA_T)"
fi 

./pmthreads.out -n 2000000 -d 2000 -t 10 -s $TYPE > /tmp/recovery_output 2>&1
cat /tmp/recovery_output | grep "successfully inserted"
nvm_consumption=$(du -h /dev/shm/pmthreads | head -n 1 | awk '{print $1}')
echo "Volume $nvm_consumption"
sleep 1 
./pmthreads.out -n 2000000 -d 2000 -t 10 -s $TYPE > /tmp/recovery_output 2>&1 
cat /tmp/recovery_output | grep "successfully recovered" 
cat /tmp/recovery_output | grep "precovery"





#CLEAN NVM
rm -rf /dev/shm/*
mkdir -p /dev/shm/pmthreads
rm -f .crashed.log
rm -f *.o *.obj
