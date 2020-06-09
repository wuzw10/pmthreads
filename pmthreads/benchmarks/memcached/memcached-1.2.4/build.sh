echo "BUILDING pmthreads.out"
make -f pmthreads.mk clean > /tmp/build_log 2>&1  
make -f pmthreads.mk  > /tmp/build_log 2>&1
if [ ! -f pmthreads.out ]
then
	cat /tmp/build_log
fi



echo "BUILDING pthread.out"
make -f pthread.mk clean  > /tmp/build_log 2>&1
make -f pthread.mk  > /tmp/build_log 2>&1
if [ ! -f pthread.out ]
then
	cat /tmp/build_log
fi


echo "BUILDING pmthreads-verbose.out"
make -f pmthreads-verbose.mk clean  > /tmp/build_log 2>&1
make -f pmthreads-verbose.mk  > /tmp/build_log 2>&1
if [ ! -f pmthreads-verbose.out ]
then
	cat /tmp/build_log
fi



cp pmthreads.out ../figure-7/
cp pthread.out ../figure-7/

cp pmthreads-verbose.out ../figure-8/
cp pmthreads-verbose.out ../figure-9/
cp pmthreads-verbose.out ../table-2/
