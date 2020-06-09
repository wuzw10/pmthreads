rm -f .crashed.log
#numactl --cpunodebind=1 
./$1 -l 127.0.0.1 -t 2 -p 11215  > .memcached.log 2>&1  & 
pid=$!
echo $pid > .PID
