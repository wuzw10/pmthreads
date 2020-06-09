PARSEC_HOME=/root/pmthreads/benchmarks/parsec/tests 
SCRIPT_HOME=/root/pmthreads/scripts/fig4a
cd $PARSEC_HOME
./figure-4a.sh > /tmp/parsec-output 2>&1  
retval="$?"
if [ "$retval" -ne 0 ]
then 
	echo "something wrong"
else
	echo "normal exit"
fi 
#cat /tmp/parsec-output 
# 17m21.606s 


cd $SCRIPT_HOME
./draw.sh 
