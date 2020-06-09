MEMCACHED_HOME=/root/pmthreads/benchmarks/memcached/figure-8/
SCRIPT_HOME=/root/pmthreads/scripts/fig8


for DELTA_T in 10 20 40 100 200 500
do
	output=/tmp/fig8-$DELTA_T-output
	cd $MEMCACHED_HOME 
	./exec.sh $DELTA_T > $output 2>&1
	retval="$?"
	if [ "$retval" -ne 0 ]
	then
		cat $output
		exit $retval
	else
		#do plot 
		cd $SCRIPT_HOME 
		./draw.sh $DELTA_T
		#cat $output
	fi
done
#takes 1m10.225s 
