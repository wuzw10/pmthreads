for RATHREADIO in 1:9 1:1 9:1
do
	for CONFIG in pthread pmthreads
	do

		if [ $CONFIG == "pmthreads" ]
		then
			for DELTA in 10 20 40 100 200 500  999999999
			do
				for THREAD in 1 2 4 8 10 
				do 	       
					./exec.sh $CONFIG $THREAD $RATHREADIO $DELTA
				done
			done
		else
			for THREAD in 1 2 4 8 10 
			do 	       
				./exec.sh $CONFIG $THREAD $RATHREADIO 
			done 
		fi
	done 
done


