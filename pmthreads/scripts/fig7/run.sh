MEMCACHED_HOME=/root/pmthreads/benchmarks/memcached
SCRIPTS_HOME=/root/pmthreads/scripts/fig7

for RATHREADIO in 1:9 1:1 9:1
do
	cd $MEMCACHED_HOME/figure-7 
	sufix=$(echo $RATHREADIO | tr ":" "-")
	output=/tmp/figure-7-$sufix 
	echo ' ' > $output 
	
	for CONFIG in pthread pmthreads
	do
		if [ $CONFIG == "pmthreads" ]
		then
			for DELTA in 10 20 40 100 200 500  999999999
			do
				for THREAD in 1 2 4 8 10
				do
					./exec.sh $CONFIG $THREAD $RATHREADIO $DELTA >> $output 2>&1 
					retval="$?"
					if [ "$retval" -ne 0 ]
					then
						echo "$CONFIG $THREAD $RATHREADIO $DELTA: something wrong"
						cat $output
						exit
					fi
				done
			done
		else
			for THREAD in 1 2 4 8 10
			do
				./exec.sh $CONFIG $THREAD $RATHREADIO >> $output 2>&1
				retval="$?"
				if [ "$retval" -ne 0 ]
				then
					echo "$CONFIG $THREAD $RATHREADIO: something wrong"
					cat $output
					exit
				fi
			done
		fi
	done
	#Plot data 
	cd $SCRIPTS_HOME  
	./draw.sh $output > output.py 
	cat output.py 
	python draw.py 
done
#33m17.120s
