MEMCACHED_HOME=/root/pmthreads/benchmarks/memcached/figure-9/
SCRIPT_HOME=/root/pmthreads/scripts/fig9


echo "import numpy as np" > output.py 
echo "figname='/tmp/fig9.pdf'" >> output.py 
echo "data={}" >> output.py

for NUM_THREADS in 1 2 4 8 10
do
	cd $MEMCACHED_HOME
	#touch /tmp/fig9-t$NUM_THREADS-output
	./exec.sh $NUM_THREADS > /tmp/fig9-t$NUM_THREADS-output 2>&1 
	retval="$?"
	if [ "$retval" -ne 0 ]
	then
		echo "something wrong. please retry. "
		cat $output
		exit $retval
	else
		cd $SCRIPT_HOME
		#do plot 
		./draw.sh $NUM_THREADS  >> output.py 
	fi
done
python draw.py 
# takes 	1m0.134s 
