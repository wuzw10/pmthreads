config=$1 

if [ $# -eq 0 ] 	
then
	config=pthread
fi 	

echo "building with $config"

mkdir -p bin 
rm -f ./parsec/bin/*
make -f Makefile.$config clean 
make -f Makefile.$config
cp ./parsec/bin/ferret-pthreads ./bin/$config.out
