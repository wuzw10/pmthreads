#run.sh EVALUATION $READ_RATIO
EVALUATION=$1
if [ -z "$1" ]
then 
	EVALUATION=1
fi

READ_RATIO=$2
if [ -z "$2" ]
then 
	READ_RATIO=0.5
fi


online=$(getconf _NPROCESSORS_ONLN)
if [ $online -le 4 ]
then #(0,4]
	T_NUMS="1 2 4"
elif [ $online -le 8  ]
then #(4, 8]
	T_NUMS="1 2 4 8"
elif [ $online -le 16  ]
then #(8, 16]
	T_NUMS="1 2 4 8 10 16"
elif [ $online -le 32  ]
then #( 16, 32]
	T_NUMS="1 2 4 8 10 16 20 32 40"
else #(32, 32+)
	T_NUMS="1 2 4 8 10 16 20 32 40"
fi

for CONFIG in pthread pmthreads-zero pmthreads-i pmthreads-p atlas nvthreads mnemosyne
do 
	for NUM_THREADS in $T_NUMS  #1 2 4 8 10 16 20 32 40 
	do 
		./exec.sh $CONFIG $NUM_THREADS $READ_RATIO $EVALUATION
	done
done
