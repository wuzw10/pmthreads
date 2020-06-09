STRESSTESTS_HOME=/root/pmthreads/benchmarks/stress-test
SCRIPTS_HOME=/root/pmthreads/scripts/fig5-6
for Fig in 5 6
do	
	for R_RATIO in 0.9 0.5 0.1 
	do 
		cd $STRESSTESTS_HOME
		output=/tmp/figure-$Fig-$R_RATIO
		./figure-$Fig.sh $R_RATIO > $output 2>&1
		retval="$?"
		if [ "$retval" -ne 0 ]
		then
			echo "figure-$Fig-$R_RATIO: something wrong" 
			cat $output 
			exit
		else
			echo "figure-$Fig-$R_RATIO: normal exit"
			#do plot 
			cd $SCRIPTS_HOME
			./draw.sh $Fig $R_RATIO > output.py  
			cat output.py 
			python draw.py 
		fi
	done 
done 
#31m55.677s 
