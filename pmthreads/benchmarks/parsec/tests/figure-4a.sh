NUM_THREADS=$1

if [ -z "$1" ]
then 
	NUM_THREADS=$(getconf _NPROCESSORS_ONLN)
fi 

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
		./exec.sh $C $NUM_THREADS
	done 
	cd $MAIN
done
