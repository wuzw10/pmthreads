TYPE=$1
if [ -z "$1" ]
then
	TYPE=0
fi

#CLEAN NVM
rm -rf /dev/shm/*
mkdir -p /dev/shm/pmthreads
rm -f .crashed.log

export NVM_LATENCY="100"
export CPU_FREQ_MHZ="2000"


if [ ! -f atlas.out ]
then
	echo "atlas.out not exist, now building ..."
	make -f atlas.mk clean > /dev/null 2>&1 
	make -f atlas.mk > /tmp/build_log 2>&1 

	if [ ! -f atlas.out ]
	then
		cat /tmp/build_log
	fi
fi 

if [ $TYPE == "0" ]
then 
	echo "LINKED LIST RECOVERY TEST WITH ATLAS" 
else
	echo "B+TREE RECOVERY TEST WITH ATLAS"
fi 

export TIMEFORMAT='TIME IS: %3R seconds'
./atlas.out -n 2000000 -d 2000 -t 10  -s $TYPE > /tmp/recovery_output  2>&1
cat /tmp/recovery_output | grep "successfully inserted DRAM elements"
user=$(whoami)
nvm_consumption=$(du -h /dev/shm/$user | head -n 1 | awk '{print $1}')
echo "Volume $nvm_consumption"
sleep 1
(time $HOME/pmthreads/third-parties/atlas/runtime/build2/tools/recover atlas.out) > /tmp/output 2>&1 && sleep 1 
num_recovered=$(cat /tmp/recovery_output | grep "successfully inserted DRAM elements" | awk '{print $5}')
recovery_time=$(cat /tmp/output  | grep "TIME IS" | cut -d " " -f3- )
echo "successfully recovered $num_recovered elements"

echo  "ATLAS recovery taks $recovery_time"

#CLEAN NVM
rm -rf /dev/shm/*
rm -f *.o *.obj
