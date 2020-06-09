scripts_home=/root/pmthreads/scripts
cd $scripts_home 
benchs="$(ls)"
for bench in $benchs
do 
	cd $bench 
	echo "running bench $bench ... "
	./run.sh 
	cd $scripts_home
done 
