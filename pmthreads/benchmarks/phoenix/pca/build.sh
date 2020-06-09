mkdir -p obj 
mkdir -p bin 


config=$1
if [ $# -eq 0 ] 	
then
	config=pthread
fi 	


export NVM_LATENCY="100"
export PERSIST_INTERVAL="100"
export CPU_FREQ_MHZ="2500"


TEST_NAME=${PWD##*/} 

echo "building $TEST_NAME with $config" 

make $TEST_NAME-$config


mv *.out bin/

rm -f *.o *.out .crashed.log  /tmp/nvlib.crash 
rm -f obj/*
