PHOENIX_HOME=/root/pmthreads/benchmarks/phoenix 
SCRIPT_HOME=/root/pmthreads/scripts/fig4b
cd $PHOENIX_HOME
././figure-4b.sh > /tmp/phoenix-output 2>&1  
retval="$?"
if [ "$retval" -ne 0 ]
then 
	echo "something wrong"
else
	echo "normal exit"
fi 

cd $SCRIPT_HOME
./draw.sh 
