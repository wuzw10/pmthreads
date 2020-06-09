FIG=$1
R_RATIO=$2
if [ -z "$1" ] || [ -z "$2" ]
then
	FIG=5       
	R_RATIO=0.9
fi 


W_RATIO=$(python -c "print(1 - $R_RATIO)")
RAW_OUTPUT=/tmp/figure-$FIG-$R_RATIO
#cat $RAW_OUTPUT | grep pthread | grep  -E "([0-9]+) threads" -o 
T_NUMS=$(cat $RAW_OUTPUT  | grep pthread | grep  -E "([0-9]+) threads" -o | grep -E "([0-9])+" -o  | tr "\n" ", ") 

unit=$( cat $RAW_OUTPUT | grep pthread | grep -E "THROUGHPUT ([0-9])+\.?([0-9])* .*" -o  | cut -d' ' -f 3  | head -n 1)
echo "unit='$unit'"
echo "nthreads=[$T_NUMS]"
echo "figname='/tmp/Figure-$FIG-PUT-$W_RATIO.pdf'"
echo "data={}"

configs="pthread pmthreads-i pmthreads-p atlas nvthreads mnemosyne"
for config in $configs
do
	throuputs=$(cat $RAW_OUTPUT | grep $config | grep -E "THROUGHPUT ([0-9])+\.?([0-9])*" -o  | grep -E "([0-9])+\.?([0-9])*" -o  | tr "\n" ", ")
	config=$(echo $config | tr "-" "_")
	echo "data['$config']=[$throuputs]"
done
