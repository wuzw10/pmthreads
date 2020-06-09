MAIN=$(pwd)
for APP in histogram  kmeans  linear_regression  matrix_multiply   pca  reverse_index  string_match  word_count
do
	cd $APP
	for C in pthread pmthreads pmthreads-mpt nvthread atlas
	do 
		echo "BUILDING $APP WITH $C"
		./build.sh $C	 > /tmp/build_log 2>&1 
		if [ ! -f ./bin/$APP-$C.out ]
		then
			cat /tmp/build_log
			exit
		fi
	done
	cd $MAIN
done
