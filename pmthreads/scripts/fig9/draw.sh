T=$1
if [ -z "$1" ]
then 
	T=4
fi 

RAWDATA=/tmp/fig9-t$T-output
timings=$(cat $RAWDATA | grep "cross_barrier" | cut -d ' ' -f 3  | tr "\n" ", " ) 
echo "data[$T]=np.array([$timings])" 
