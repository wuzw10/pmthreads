rm -f *.so 
for c in pmthreads pmthreads-mpt pmthreads-zero pmthreads-malloc pmthreads-mpt-malloc pmthreads-verbose
do 
	echo "BUILDING lib$c.so ..."
	python build.py $c > /tmp/build_log 2>&1
	if [ ! -f lib$c.so ]
	then
		cat /tmp/build_log
		exit 
	fi 
done
