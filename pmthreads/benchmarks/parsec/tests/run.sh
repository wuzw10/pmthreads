MAIN=$(pwd)
for APP in blackscholes canneal  dedup  ferret  streamcluster  swaptions
do
	echo "EVALUATE $APP"
	cd $APP
		
	for C in pthread pmthreads pmthreads-mpt nvthread atlas 
	do 
		if [ $C == "pmthreads" ]
		then
			echo -n "  pmthreads-i  "
		elif [ $C == "pmthreads-mpt" ]
		then
			echo -n "  pmthreads-p  "
		else
			echo -n "  $C  "
		fi
		./exec.sh $C 20
	done 
	cd $MAIN
done
