READ_RATIO=$1
if [ -z "$1" ]
then
	READ_RATIO=0.5
fi

./run.sh 0 $READ_RATIO
