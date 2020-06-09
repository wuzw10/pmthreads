MAIN=$(pwd)
for APP in histogram  kmeans  linear_regression  matrix_multiply   pca  reverse_index  string_match  word_count
do
	echo "EVALUATE $APP"
	cd $APP
	cp $HOME/pmthreads/benchmarks/phoenix/exec.sh . 
	for C in pthread #pmthreads pmthreads-mpt nvthread atlas 
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
		./exec.sh $C
	done 
	cd $MAIN
done
