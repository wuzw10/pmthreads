RAWDATA=$1
if [ -z "$1" ]
then
	echo "no input, exit ..."
	exit 1       
fi 

configs=$(cat $RAWDATA  | grep -E  "DELTA_T=([0-9])+" -o  | tr "\n" "\t")
configs="$configs atlas" 
volume=$(cat $RAWDATA   | grep Volume | cut -d' ' -f 2 | tr "\n" "\t")
timings=$(cat $RAWDATA | grep -E "([0-9])+\.?([0-9])*(.?)*seconds" -o  | tr "\n" "\t" | sed 's/mili seconds/ms/g' | sed 's/seconds/s/g')
inserted=$(cat $RAWDATA   | grep inserted  | grep -E "([0-9])+" -o  | tr "\n" "\t")
recovered=$(cat $RAWDATA   | grep recovered | grep -E  "([0-9])+"  -o | tr "\n" "\t")

echo $configs
echo -n "inserted: "
echo $inserted 
echo -n "recovered: "
echo $recovered 
echo $timings
echo $volume

