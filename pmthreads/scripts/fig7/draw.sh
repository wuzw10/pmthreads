RAWDATA=$1 
if [ -z "$1" ]
then
	echo "no input file, exit ... "
	exit 
fi 

N_THREADS=$(cat $RAWDATA | grep pthread | grep  -E "([0-9]+) threads" -o | grep -E "([0-9])+" -o  | tr "\n" ", ")
DELTAS=$(cat $RAWDATA | grep "1 thread" | grep -E "DELTA_T ([0-9]+)" -o | grep -E "([0-9])+" -o | tr "\n" " ")
echo "data={}"
echo "nthreads=[$N_THREADS]"
#echo "deltas=[$DELTAS]"
echo "unit='ops/seconds'"

echo "figname='$1.pdf'"



throughputs=$(cat $RAWDATA | grep pthread |  grep -E "THROUGHPUT ([0-9])+\.?([0-9])*" -o | grep -E "([0-9])+\.?([0-9])*" -o | tr "\n" ", ")
echo "data['pthread']=[$throughputs]"
for D in $DELTAS
do
	throughputs=$(cat $RAWDATA  | grep "DELTA_T $D " |  grep -E "THROUGHPUT ([0-9])+\.?([0-9])*" -o | grep -E "([0-9])+\.?([0-9])*" -o | tr "\n" ", ")
	if [ $D == "999999999" ]
	then
		D="inf"
	fi
	echo "data['pmthreads-$D']=[$throughputs]"
done 
