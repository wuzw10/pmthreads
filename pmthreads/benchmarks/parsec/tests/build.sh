MAIN=$(pwd)
for APP in blackscholes canneal  dedup  ferret  streamcluster  swaptions
do
	cd $APP
	for C in pthread pmthreads pmthreads-mpt nvthread atlas
	do
		echo "BUILDING $APP with $C"
		./build.sh $C  > /tmp/build_log 2>&1
		if [ ! -f ./bin/$C.out ]
		then
			cat /tmp/build_log
			exit
		fi
	done
	cd $MAIN
done
