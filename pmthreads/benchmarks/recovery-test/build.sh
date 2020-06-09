echo "BUILDING pmthreads.out"
make -f pmthreads.mk clean > /dev/null 2>&1 
make -f pmthreads.mk > /tmp/build_log 2>&1 

if [ ! -f pmthreads.out ]
then
	cat /tmp/build_log
fi

echo "BUILDING atlas.out"

make -f atlas.mk clean > /dev/null 2>&1
make -f atlas.mk > /tmp/build_log 2>&1

if [ ! -f atlas.out ]
then
	cat /tmp/build_log
fi
