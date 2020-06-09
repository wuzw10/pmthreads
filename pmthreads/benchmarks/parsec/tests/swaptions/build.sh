config=$1 

if [ $# -eq 0 ] 	
then
	config=pthread
fi 	

echo "building with $config"

echo "CONFIG=$config" > config.mk 

export NVM_LATENCY="100"
export PERSIST_INTERVAL="100"
export CPU_FREQ_MHZ="2500"

cat $HOME/pmthreads/benchmarks/parsec/Makefile.common  > Makefile 
cat $HOME/pmthreads/benchmarks/parsec/Makefile.$config >> Makefile  

make binary 
rm -f *.o *.out .crashed.log  /tmp/nvlib.crash
