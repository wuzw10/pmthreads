DELTA=$1
if [ -z "$1" ]
then
	echo "no input ... "
	exit 1
fi 

RAWDATA=/tmp/fig8-$DELTA-output

echo "import numpy as np" > output.py 
echo "delta=$DELTA" >> output.py
echo "figname='$RAWDATA.pdf'" >> output.py
ticks=$(cat $RAWDATA | grep "COMMIT STATISTICS" | cut -d' ' -f 6 | tr "\n" ", ")
dram=$(cat $RAWDATA | grep "COMMIT STATISTICS"  | cut -d ' ' -f 10 | tr "\n" ", ")
nvm=$(cat $RAWDATA  | grep "COMMIT STATISTICS" | cut -d ' ' -f 14 | tr "\n" ", ")
saved=$(cat $RAWDATA  | grep "COMMIT STATISTICS" | cut -d ' ' -f 19 | tr "\n" ", ")
echo "ticks=np.array([$ticks])" >> output.py 
echo "dram=np.array([$dram])" >> output.py
echo "nvm=np.array([$nvm])" >> output.py
echo "saved=np.array([$saved])" >> output.py 
#cat output.py  

python draw.py
