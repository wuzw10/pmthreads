config=$1 

if [ $# -eq 0 ] 	
then
	config=pthread
fi 	



mkdir -p obj 
mkdir -p bin 

make dedup-$config > /tmp/build_output 2>&1 
if [ ! -f ./dedup-$config.out ]
then
	cat /tmp/build_output	
else
	cp ./dedup-$config.out ./bin/$config.out
fi 
