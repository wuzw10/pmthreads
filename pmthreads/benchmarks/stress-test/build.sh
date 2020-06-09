CONFIG=$1
#pmthreads-i pmthreads-p pmthreads-zero atlas nvthreads pthread
if [ -z "$1" ] 
then
	CONFIG="all"
fi 

if [ $CONFIG == "all" ] || [ $CONFIG == "mnemosyne" ]
then
	echo "build mnemosyne ... "
	cd $HOME/pmthreads/third-parties/mnemosyne-gcc-alps/usermode
	scons -c > /tmp/build_log 2>&1 && sleep 1
	scons --build-example=stress-test > /tmp/build_log 2>&1  && sleep 1 
	menmosyne_stress_test=$HOME/pmthreads/third-parties/mnemosyne-gcc-alps/usermode/build/examples/stress-test/stress-test
	if [ ! -f $menmosyne_stress_test  ]
	then
		cat /tmp/build_log
		exit 
	else
		echo "done."
	fi 
	cd $HOME/pmthreads/benchmarks/stress-test
	if [ $CONFIG == "mnemosyne" ]
	then 
		exit
	fi
fi 

if [ $CONFIG == "all" ]
then
	rm -f *.out 
	for c in pmthreads-i pmthreads-p pmthreads-zero atlas nvthreads pthread
	do
		echo "building $c ... "
		(cp $c.mk configs.mk && make clean && make -j 20) > /tmp/build_log 2>&1 

		if [ ! -f $c.out ]
		then
			cat /tmp/build_log
			exit 
		else
			ls -lht $c.out
		fi
	done
else
	c=$CONFIG
	echo "building $c ... "
	(cp $c.mk configs.mk && make clean && make -j 20) > /tmp/build_log 2>&1
	if [ ! -f $c.out ]
	then
		cat /tmp/build_log
		exit 
	else
		ls -lht $c.out
	fi
fi

rm -f *.o *.obj
